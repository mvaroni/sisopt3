#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "libdisksimul.h"
#include "filesystem.h"

/**
 * @brief Format disk.
 * 
 */
int fs_format(){
	int ret, i;
	struct table_directory root_dir;
	struct sector_0 sector0;
	struct sector_data sector;
	
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 1)) != 0 ){
		return ret;
	}
    	
	memset (&sector0, 0, sizeof(struct sector_0));
	
	/* first free sector. */
	sector0.free_sectors_list = 2;
	
	ds_write_sector(0, (void*)&sector0, SECTOR_SIZE);
	
	memset(&root_dir, 0, sizeof(root_dir));
	
	ds_write_sector(1, (void*)&root_dir, SECTOR_SIZE);
	
	/* Create a list of free sectors. */
	memset(&sector, 0, sizeof(sector));
	
	for(i=2;i<NUMBER_OF_SECTORS;i++){
		if(i<NUMBER_OF_SECTORS-1){
			sector.next_sector = i+1;
		}else{
			sector.next_sector = 0;
		}
		ds_write_sector(i, (void*)&sector, SECTOR_SIZE);
	}
	
	ds_stop();
	
	printf("Disk size %d kbytes, %d sectors.\n", (SECTOR_SIZE*NUMBER_OF_SECTORS)/1024, NUMBER_OF_SECTORS);
	
	return 0;
}

/**
 * @brief Create a new file on the simulated filesystem.
 * @param input_file Source file path.
 * @param simul_file Destination file path on the simulated file system.
 * @return 0 on success.
 */
int fs_create(char* input_file, char* simul_file){
    int ret, qtd_setores, prox_setor;
    struct sector_0 sector0;
    struct file_dir_entry file;
    struct sector_data sectorD;
    char *token, *virtual_end;
    char file_data[508];

	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){		
        return ret;
	}

	/**
	 * INICIO do trecho onde tratamos o caminho do arquivo simulado 
	 * e salvamos os dados da struct file_dir_entry
	 */

    memset(file_data, 0, sizeof(file_data));
	memset (&sector0, 0, sizeof(struct sector_0));
	memset (&sectorD, 0, sizeof(struct sector_data));
	memset (&file, 0, sizeof(struct file_dir_entry));

    ds_read_sector(0, (void*)&sector0, SECTOR_SIZE);

    virtual_end = simul_file;

    // Se o primeiro caractere lido for barra não é um arquivo
    if(virtual_end[strlen(virtual_end)-1] == '/'){
        printf("Caminho passado é apenas um diretório...\n");
        return ret;
    }


	// 'file' é um arquivo
    file.dir = 0;

    // Endereço virtual recebe a string invertida
    virtual_end = reverse(virtual_end);

    // strtok pega o nome do arquivo usando o delimitador /, depois reinverte
    token = strtok(virtual_end, "/");
    token = reverse(token);

    // strcpy passa para 'file.name' o valor do token, e depois zeramos ele
    strcpy(file.name, token);
    token = strtok(NULL,"/");

	// Descobrimos o tamanho do arquivo e salvamos em 'file.size_bytes'
	FILE* fp;
	fp = fopen(input_file, "rb");
	file.size_bytes = filesize(fp);

	// Utilizando o tamanho do arquivo já podemos identificar quantos 
	//setores serão necessários alocar para que o arquivo seja salvo
	qtd_setores = ((file.size_bytes + 508 - 1) / 508);

	// Salvamos em 'file.sector_start' qual o setor inicial que contém os dados
	file.sector_start = sector0.free_sectors_list;

    // Enquanto houverem setores na varíavel 'qtd_setores' utilizamos o método
    //fread() para ler 508 bytes do arquivo e alocá-los no setor de dados
    if(fp){
        // Lê o primeiro setor livre e "aloca" na variável sectorD
        ds_read_sector(file.sector_start, (void*)&sectorD, SECTOR_SIZE);
        fread(file_data, 508, 1, fp);
        // Escreve no primeiro setor livre os primeiros 508 bytes
        ds_write_sector(file.sector_start, file_data, SECTOR_SIZE);

        // Decrementamos contador
        qtd_setores--;

        /** Neste while, enquanto houverem setores para serem escritos e dados no arquivo
        *continuamos a ler o arquivo e escrever nos setores
        *** A varíavel contadora decrementa e setamos 'prox_setor' para o próximo setor livre
        */
        while(qtd_setores > 0){
            // A variável 'prox_setor' recebe o próximo setor, partindo da lista de setores livres
            prox_setor = sectorD.next_sector;
            ds_read_sector(prox_setor, (void*)&sectorD, SECTOR_SIZE);
            fread(file_data, 508, 1, fp);
            ds_write_sector(prox_setor, file_data, SECTOR_SIZE);
            qtd_setores--;
        }
		// Setor 0 recebe o próximo setor livre
        sector0.free_sectors_list = sectorD.next_sector;
		printf("asdasdasd %d\n",sector0.free_sectors_list);

		// Último setor do arquivo alocado aponta para o setor 0
		sectorD.next_sector = 0;
    }

	// Fechamos o arquivo
    fclose(fp); fp = NULL;

	/**
	 * FIM do trecho onde tratamos o caminho do arquivo simulado 
	 * e salvamos os dados da struct file_dir_entry
	 */
    
	ds_stop();

	return 0;
}

/**
 * @brief Read file from the simulated filesystem.
 * @param output_file Output file path.
 * @param simul_file Source file path from the simulated file system.
 * @return 0 on success.
 */
int fs_read(char* output_file, char* simul_file){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to read a file from the simulated filesystem. */
    

    
	ds_stop();
	
	return 0;
}

/**
 * @brief Delete file from file system.
 * @param simul_file Source file path.
 * @return 0 on success.
 */
int fs_del(char* simul_file){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code delete a file from the simulated filesystem. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief List files from a directory.
 * @param simul_file Source file path.
 * @return 0 on success.
 */
int fs_ls(char *dir_path){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to show files or directories. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Create a new directory on the simulated filesystem.
 * @param directory_path directory path.
 * @return 0 on success.
 */
int fs_mkdir(char* directory_path){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to create a new directory. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Remove directory from the simulated filesystem.
 * @param directory_path directory path.
 * @return 0 on success.
 */
int fs_rmdir(char *directory_path){
	int ret;
	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* Write the code to delete a directory. */
	
	ds_stop();
	
	return 0;
}

/**
 * @brief Generate a map of used/available sectors. 
 * @param log_f Log file with the sector map.
 * @return 0 on success.
 */
int fs_free_map(char *log_f){
	int ret, i, next;
	//struct root_table_directory root_dir;
	struct sector_0 sector0;
	struct sector_data sector;
	char *sector_array;
	FILE* log;
	int pid, status;
	int free_space = 0;
	char* exec_params[] = {"gnuplot", "sector_map.gnuplot" , NULL};


	if ( (ret = ds_init(FILENAME, SECTOR_SIZE, NUMBER_OF_SECTORS, 0)) != 0 ){
		return ret;
	}
	
	/* each byte represents a sector. */
	sector_array = (char*)malloc(NUMBER_OF_SECTORS);

	/* set 0 to all sectors. Zero means that the sector is used. */
	memset(sector_array, 0, NUMBER_OF_SECTORS);
	
	/* Read the sector 0 to get the free blocks list. */
	ds_read_sector(0, (void*)&sector0, SECTOR_SIZE);
	
	next = sector0.free_sectors_list;
	printf("   %d   ",next);
	while(next){
		/* The sector is in the free list, mark with 1. */
		sector_array[next] = 1;
		
		
		/* move to the next free sector. */
		ds_read_sector(next, (void*)&sector, SECTOR_SIZE);
		
		next = sector.next_sector;
		
		free_space += SECTOR_SIZE;
	}

	/* Create a log file. */
	if( (log = fopen(log_f, "w")) == NULL){
		perror("fopen()");
		free(sector_array);
		ds_stop();
		return 1;
	}

	/* Write the the sector map to the log file. */
	for(i=0;i<NUMBER_OF_SECTORS;i++){
		if(i%32==0) fprintf(log, "%s", "\n");
		fprintf(log, " %d", sector_array[i]);
	}

	fclose(log);

	/* Execute gnuplot to generate the sector's free map. */
	pid = fork();
	if(pid==0){
		execvp("gnuplot", exec_params);
	}
	/* Wait gnuplot to finish */
	wait(&status);

	free(sector_array);

	ds_stop();

	printf("Free space %d bytes.\n", free_space);
	
	return 0;
}

char* reverse(char* str){
    int i = 0;
    int j = strlen(str) - 1;
    char temp;

    while(i < j){
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }

    return str;
}

/* Descobre o tamanho do arquivo */
int filesize(FILE* fp){
	int sz;

	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	return sz;
}
