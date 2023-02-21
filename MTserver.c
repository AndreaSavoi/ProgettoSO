#include "Server.h"
#include "Helper.h"

s_node *head;

void *thread_func(void *param) {

    int c_socket = (long int)param;
    char *username, *usernameV;
    char *password, *passwordV;
    char buffer[1024];
    char tempBuf[1024];
    char bufferS[1024];
    char bufferB[1024];
    int seatCheck = 0;
    char reservedSeats[1024];
    s_node *curr;
    int x, y, fd, countV = 0;
    char *p1, *p2;

    login_start:
    if(Readline(c_socket, buffer, MAX_DIM - 1) <=0) {				//Receive selected operation number from client
        printf("[-]Receive failed\n");
        pthread_exit(NULL);
    }

    printf("Buffer 1 : %s", buffer);
    puts("ciao\n");

    buffer[strcspn(buffer, "\n")] = 0;

    printf("Buffer 2 : %s", buffer);
    puts("ciao\n");

    operation = atoi(buffer);

    switch(operation) {
//
//CASE 1: LOGIN
//
	 case 1:
         puts("CASE 1 LOGIN");
            if(Readline(c_socket, buffer, MAX_DIM - 1) <= 0) {			//Receive username and password entered by user to check if exists in fs-db
                puts("[-]Receive failed.\n");
                pthread_exit(NULL);
            }

            printf("Buffer 3 : %s", buffer);
            puts("ciao\n");

            buffer[strcspn(buffer, "\n")] = 0;

            printf("Buffer 4 : %s", buffer);
            puts("ciao\n");

            strcpy(bufferS, buffer);

            username = strtok(bufferS, ";");

            password = strtok(NULL, ";");

            fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0666);	//Opening file to read datas
            fileL = fdopen(fd, "w+");

            countV = 0;

            while(fgets(tempBuf, sizeof(tempBuf), fileL)) {             //Checking all fs-database entries for corresponding username and password

                tempBuf[strcspn(tempBuf, "\n")] = 0;

                usernameV = strtok(tempBuf, ";");

                passwordV = strtok(NULL, ";");

                if (!strcmp(username, usernameV)) {                     //Checking the username with the corresponding entry
                    if (!strcmp(password, passwordV)){                  //Checking the password with the corresponding entry
                        countV = 1;
                        break;
                    }
                }
            }

            if(countV) {                                                //Account found, sending ack to client
                sprintf(buffer, "%s\n", "1");
                if (Writeline(c_socket, buffer, strlen(buffer)) < 0) {
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }
            } else if (!countV) {					//Acccount not found, sending ack to client
                sprintf(buffer, "%s\n", "0");
                if (Writeline(c_socket, buffer, strlen(buffer)) < 0) {
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }
                goto login_start;					//Since account hasn't been found, user can retry to log-in or sign-up
            }
            break;
//
//CASE 2:SIGN-UP
//
        case 2:
            puts("CASE 2 LOGIN");
            if(Readline(c_socket, buffer, MAX_DIM - 1) <= 0) {			//Receive username and password for registration from the client
                puts("[-]Receive failed\n");
                pthread_exit(NULL);
            }
            printf("Buffer 5 : %s", buffer);
            puts("ciao\n");

            buffer[strcspn(buffer, "\n")] = 0;

            printf("Buffer 6 : %s", buffer);
            puts("ciao\n");

            fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0666);
            fileL = fdopen(fd, "w+");

            if(fileL == NULL) {
                puts("[-]Fopen failed\n");
                pthread_exit(NULL);
            }

            strcpy(bufferS, buffer);

            username = strtok(bufferS, ";");

            countV = 0;

            while(fgets(tempBuf, sizeof(tempBuf), fileL) && !countV) {	//Checking all fs-db entries to see if the username already exists

                puts("inside while");

                tempBuf[strcspn(tempBuf, "\n")] = 0;

                usernameV = strtok(tempBuf, ";");

                if (!strcmp(username, usernameV)) {
                    countV++;
                } else {
                    countV = 0;
                }
            }

            fclose(fileL);

            if(!countV) {						//Username doesn't exist in fs-db so the user can register with said username	
                fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0666);
                fileL = fdopen(fd, "w+");
                fprintf(fileL, "%s\n",  buffer);			//Insert username and password into fs-db
                fflush(fileL);
                fclose(fileL);
                sprintf(buffer, "%s\n", "1");				//Sending ack to client to confirm the registration
                if (Writeline(c_socket, buffer, strlen(buffer)) <= 0) {
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }
                printf("Buffer : %s", buffer);
                puts("ciao");
            } else {
                sprintf(buffer, "%s\n", "0");				//Sending ack to client saying the username is already taken
                if (Writeline(c_socket, buffer, strlen(buffer)) <= 0) {
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }
                printf("Buffer : %s", buffer);
                puts("ciao");
                goto login_start;					//Since username was taken, user can retry to sign-up or login
            }
            break;
//
//CASE 3: EXIT
//
        case 3:
            pthread_exit(NULL);
    }

    sprintf(buffer, "%d;%d\n", rows, r_seats);

    if(Writeline(c_socket, buffer, strlen(buffer)) <= 0) {				//Sending rows and seats to the client
       	puts("[-]Send error.\n");
       	pthread_exit(NULL);
    }

    while(1){
        puts("[!]Waiting for an input\n");
        if(Readline(c_socket, buffer, MAX_DIM - 1) < 0) {			//Receive selected operation number from client
            puts("[-]Receive failed.\n");
            pthread_exit(NULL);
        }

        printf("Buffer 7 : %s", buffer);
        puts("ciao\n");

        buffer[strcspn(buffer, "\n")] = 0;

        printf("Buffer 8 : %s", buffer);
        puts("ciao\n");

        operation = atoi(buffer);

        switch(operation) {
//
//CASE 1: PRINT THE MAP OF THE SEATS
//
            case 1:
                puts("CASE 1 T");
                sprintf(buffer, "%d\n", n_counter);			//Sending the number of the current reserved seats to client
                if(Writeline(c_socket, buffer, strlen(buffer)) <= 0) {
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }

                printf("[+]Sending data to client.\n");			//Sending the coordinates of the reserved seats to the client
                curr = head;
                while(curr->s_j >= 0 && curr->s_i >= 0 && curr) {
                    sprintf(buffer, "%d;%d\n", curr->s_i, curr->s_j);
                    if(Writeline(c_socket, buffer, strlen(buffer)) < 0) {
                        puts("[-]Send failed\n");
                    }
                    curr = curr->next;
                }
                puts("[+]Data to client successfully sent.\n");
                break;
//
//CASE 2: RESERVE A SEAT
//
            case 2:
                puts("CASE 2 T");
            reserv_routine:
                if(Readline(c_socket, buffer, MAX_DIM) <= 0) { 		//Receiving the number of seats the user wants to reserve
                    puts("[-]Receive failed.\n");
                    pthread_exit(NULL);
                }

                printf("Buffer 9 : %s", buffer);
                puts("ciao\n");

                buffer[strcspn(buffer, "\n")] = 0;

                printf("Buffer 10 : %s", buffer);
                puts("ciao\n");

                tokind = atoi(buffer);
                code++;


                if(Readline(c_socket, buffer, MAX_DIM - 1) <= 0) {		//Receiving the coordinates of the seats the user wants to reserve
                    puts("[-]Receive failed.\n");
                    pthread_exit(NULL);
                }

                printf("Buffer 11 : %s", buffer);
                puts("ciao\n");

                buffer[strcspn(buffer, "\n")] = 0;

                printf("Buffer 12 : %s", buffer);
                puts("ciao\n");

                strncpy(unModBuff, buffer, 1024);

                singleSeatBuff = strtok(buffer, delim);			//Selecting only on seat

                strncpy(tempBuf, singleSeatBuff, strlen(singleSeatBuff)+1);

		seatCheck = 0;

                while(singleSeatBuff) {					//As long as a seat exists in the reservation

                    strncpy(tempBuf, singleSeatBuff, strlen(singleSeatBuff)+1);

                    x = singleSeatBuff[0] - 65;
                    p1 = singleSeatBuff;
                    p2 = singleSeatBuff + 1;
                    if(memmove(p1, p2, strlen(singleSeatBuff)) == NULL) {	//Separating the row from the seat number
                        puts("[-]Memmove failure.\n");
                        pthread_exit(NULL);
                    }

                    y = atoi(singleSeatBuff);

                    curr = head;

                    while(curr) {					//Check if the seat has been already reserved by someone else

                        if(curr->s_i == x && curr->s_j == y) {
                            seatCheck = 1;
                            strcat(reservedSeats, " ");
                            strcat(reservedSeats, tempBuf);
                            break;
                        }
                        curr = curr->next;
                    }

                    singleSeatBuff = strtok(NULL, delim);		//Checking the next seat in the reservation

                }

                curr = head;

                if(seatCheck == 1) {
                    puts("[-]Reservation Failure (Some of the selected seats are already reserved)\n");
                    strcat(reservedSeats, "\n");
                    if(Writeline(c_socket, reservedSeats, strlen(reservedSeats)) <= 0) {	//Sending ack to client saying the reservation failed
                        puts("[-]Send failed\n");
                        pthread_exit(NULL);
                    }
                    goto reserv_routine;
                }
                else {
                    puts("[+]Check Done.... Starting to reserve the seats\n");
                    strcpy(buffer, "success\n");
                    if(Writeline(c_socket, buffer, strlen(buffer)) <= 0){		//Sending ack to client saying the seats are free
                        puts("[-]Send failed\n");
                        pthread_exit(NULL);
                    }
                }

                singleSeatBuff = strtok(unModBuff, delim);


                for(int i=0; i<tokind; i++) {				//Insert reservation's seat in the list

                    s_node *new = malloc(sizeof(s_node));
                    if(!new) {
                        puts("[-]Malloc error.\n");
                        pthread_exit(NULL);
                    }

                    new->usr=username;
                    new->s_i = singleSeatBuff[0] - 65;
                    p1 = singleSeatBuff;
                    p2 = singleSeatBuff + 1;
                    if(memmove(p1, p2, strlen(singleSeatBuff)) == NULL) {
                        puts("[-]Memmove failure.\n");
                        pthread_exit(NULL);
                    }
                    new->s_j = atoi(singleSeatBuff);
                    new->s_code = code;
                    new->next = head;
                    head = new;

                    fileB = fopen(filenameB, "a+");			
                    fprintf(fileB, "%d;%d;%d;%s\n", new->s_i, new->s_j, new->s_code, new->usr);	//Insert seat in the backup file
                    fclose(fileB);


                    singleSeatBuff = strtok(NULL, delim);
                }

                n_counter += tokind;

		sprintf(buffer, "%d\n", code);

                if(Writeline(c_socket, buffer, strlen(buffer)) <= 0) { // Sending the code of the reservation to the client application
                    puts("[-]Send error.\n");
                    pthread_exit(NULL);
                }
                break;
//
//CASE 3: DELETE A RESERVATION
//
            case 3:
                puts("CASE 3 T");
                if(Readline(c_socket, buffer, MAX_DIM) < 0) { // Receive the code of the reservation to delete
                    puts("[-]Receive failure.\n");
                    pthread_exit(NULL);
                }

                printf("Buffer 13 : %s", buffer);
                puts("ciao\n");

                buffer[strcspn(buffer, "\n")] = 0;

                printf("Buffer 14 : %s", buffer);
                puts("ciao\n");

                int del_code = atoi(buffer);
		
                countV = 0;

                curr = head;

                while(curr->s_code == del_code && !strcmp(curr->usr, username)) {	//Checking if the reservation with said code exists and belongs to the user that requested the delete

                    s_node *del = curr;
                    sprintf(buffer, "%d;%d\n", curr->s_i, curr->s_j);
                    head = curr->next;
                    curr = head;
                    free(del);
                    puts("[+]Node deleted.\n");

                    if(Writeline(c_socket, buffer, strlen(buffer)) < 0) {
                        printf("[-]Send error.\n");
                        pthread_exit(NULL);
                    }
                    countV++;
                    n_counter--;
                }

                while(curr->next) {

                    while(curr->next->s_code == del_code && !strcmp(curr->next->usr, username)) {

                        s_node *del = curr->next;
			            sprintf(buffer, "%d;%d\n", del->s_i, del->s_j);
                        curr->next = curr->next->next;
                        free(del);
                        puts("[+]Node deleted.\n");
                        countV++;
                        n_counter--;

                        if(Writeline(c_socket, buffer, strlen(buffer)) <= 0) { // Sending the ack. of the remove
                            printf("[-]Send error.\n");
                            pthread_exit(NULL);
                        }
                    }
                    curr=curr->next;
                }

		
                int fdBT = open(filenameBT, O_RDWR | O_CREAT | O_APPEND | O_TRUNC, 0666);	//Creating a temporary backup file for safety reasons
                fileBT = fdopen(fdBT, "w+");
                if(!fileBT) {
                    puts("[-]Fdopen failure.\n");
                    pthread_exit(NULL);
                }

                if(fprintf(fileBT, "%d;%d\n", rows, r_seats) <= 0) {	//Insert rows and seats in the first line to respect the standard pattern of the file
                    puts("[-]Fprintf failure.\n");
                    pthread_exit(NULL);
                }

                curr = head;
                while(curr->s_code != -1) {			//Insert into temporary backup all the current node without the deleted ones

                    if(fprintf(fileBT, "%d;%d;%d;%s\n", curr->s_i, curr->s_j, curr->s_code, curr->usr) != 7 + (int)strlen(curr->usr)) {
                        puts("[-]fprintf failure.\n");
                        pthread_exit(NULL);
                    }
                    curr = curr->next;
                }

                if(fclose(fileBT) != 0) {
                    puts("[-]Fclose failure.\n");
                    pthread_exit(NULL);
                }

                fileBT = fopen(filenameBT, "r");
                if(!fileBT) {
                    puts("[-]fopen failure (fileBT)\n");
                    pthread_exit(NULL);
                }

                fileB = fopen(filenameB, "w");
                if(!fileB) {
                    puts("[-]fopen failure (fileB)\n");
                    pthread_exit(NULL);
                }

                while(fgets(bufferB, sizeof(bufferB), fileBT)) {	//Copying all the temporary backup into the standard backup
                    bufferB[strcspn(bufferB, "\n")] = 0;
                    if(fprintf(fileB, "%s\n", bufferB) != (int)strlen(bufferB) + 1) {
                        puts("[-]fprintf failure\n");
                        pthread_exit(NULL);
                    }
                }

                if(fclose(fileBT) != 0) {
                    puts("[-]Fclose failure (fileBT)\n");
                    pthread_exit(NULL);
                }

                if(fclose(fileB) != 0) {
                    puts("[-]Fclose failure (fileB)\n");
                    pthread_exit(NULL);
                }


                if(!countV) {
                    sprintf(buffer, "0\n");				//Sending ack to client if delete failed
                    if(Writeline(c_socket, buffer, strlen(buffer)) < 0) {
                        printf("[-]Send error.\n");
                        pthread_exit(NULL);
                    }
                } else if (countV > 0) {
                    sprintf(buffer, "1\n");				//Sending ack to client if delete successful
                    if(Writeline(c_socket, buffer, strlen(buffer)) <= 0) {
                        printf("[-]Send error.\n");
                        pthread_exit(NULL);
                    }
                }

                break;
//
//CASE 4: EXIT
//
            case 4:
                pthread_exit(NULL);
                break;

//
//CASE DEFAULT: INVALID INPUT
//
            default :
                printf("[-]Invalid input (= %d)\n", operation);
                pthread_exit(NULL);
        }

        if(Readline(c_socket, buffer, MAX_DIM) <= 0) {		//Receive Y or N from client to continue
            printf("[-]Receive error.\n");
            pthread_exit(NULL);
        }

        printf("Buffer 15 : %s", buffer);
        puts("ciao\n");

        buffer[strcspn(buffer, "\n")] = 0;

        printf("Buffer 16 : %s", buffer);
        puts("ciao\n");

        if(!strcmp(buffer, "N"))
            break;
    }
    return 0;
}

int main() {

    int l_socket, c_socket, ret, b_choice;
    char *tokBuf;
    char backupBuf[1024];
    pthread_t tid;
    socklen_t addr_size;
    struct sockaddr_in server;
    struct sockaddr_in client;

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);			//Mask for ignoring all signals

    /*list = malloc(sizeof(pthread_mutex_t));			//Malloc for mutex list
    login = malloc(sizeof(pthread_mutex_t));			//Malloc for mutex login

    if(list == NULL || login == NULL) {
        puts("[-]Mutex malloc error.\n");
        exit(1);
    }

    if(pthread_mutex_init(list, NULL) || pthread_mutex_init(login, NULL) || pthread_mutex_lock(list) ||
            pthread_mutex_lock(login)){				//Init and lock for both list and login
        puts("[-]Mutex init and lock error.\n");
        exit(1);
    }*/

    head = malloc(sizeof(s_node));				//Adding sentinel node to list
    if(!head) {
        puts("[-]Malloc1 Failure.\n");
        exit(1);
    }

    head->s_i = -1;
    head->s_j = -1;
    head->s_code = -1;
    head->usr = NULL;
    head->next = NULL;

    scanf_backup:
    puts("Do you want to load the data from a previous backup file?\n-) 1 to confirm\n-) 0 to refuse\n");
    b_choice = 0;
    ret = scanf("%d", &b_choice);
    if(!ret) {
        puts("[-]Incompatible input type...\n");
        getc(stdin);
        goto scanf_backup;
    }
    if(ret == EOF) {
        puts("[-]Scanf failure\n");
        pthread_exit(NULL);
    }

    if(b_choice == 1) {						//Server's user wants to load a backup file
        fileB = fopen(filenameB, "r");
        if(!fileB) {
            puts("[-]Error while opening the backup file...\n");
            exit(1);
        }

        puts("[+]Backup file opened, loading data...\n");

        fgets(backupBuf, sizeof(backupBuf), fileB);		//Adding rows and seat number from the backup file
        backupBuf[strcspn(backupBuf, "\n")] = 0;
        tokBuf = strtok(backupBuf, ";");
        rows = atoi(tokBuf);
        tokBuf = strtok(NULL, ";");
        r_seats = atoi(tokBuf);

        while(fgets(backupBuf, sizeof(backupBuf), fileB)) {	//Adding each reservation in the backup file into the list
            backupBuf[strcspn(backupBuf, "\n")] = 0;
            s_node *new = calloc(1, sizeof(s_node));
            if(!new) {
                puts("[-]Calloc failure\n");
                exit(1);
            }
            tokBuf = strtok(backupBuf, ";");
            new->s_i = atoi(tokBuf);
	    tokBuf = strtok(NULL, ";");
            new->s_j = atoi(tokBuf);
	    tokBuf = strtok(NULL, ";");
            new->s_code = atoi(tokBuf);
	    tokBuf = strtok(NULL, ";");
	    new->usr = malloc(128);
	    if((new->usr) == NULL) {
		    puts("[-]Malloc error.\n");
		    exit(1);
	    }
	    strcpy(new->usr, tokBuf);
	    new->next = head;
            head = new;
            n_counter++;
            printf("[+] Node loaded: i=%d | j=%d | code=%d | usr=%s\n", new->s_i, new->s_j, new->s_code, new->usr);
        }

	code = 0;
        s_node *curr = head;
        while(curr->s_code != -1) {
            if(curr->s_code > code)
                code = curr->s_code;
            curr = curr->next;
        }

	puts("[+]Backup completed, starting the server...\n");
    }
    else if(b_choice == 0) {					//Server's user doesn't want to load a backup file
        puts("[+] Creating a new backup file...\n");
        fileB = fopen(filenameB, "w+");				//Creating a new backup file
        if(!fileB) {
            puts("[-] fopen failure (fileB)\n");
            exit(1);
        }
        puts("[+] Creation completed, starting the server...\n");

        rows:
        puts("Welcome to the server program!\n\nHow many rows does your theater have?\n");
        ret = scanf("%d", &rows);
        if (!ret) {
            puts("[-]Incompatible type in scanf\n");
            getc(stdin);
            goto rows;
        }
        if (ret == EOF) {
            puts("[-]Scanf failure\n");
            exit(1);
        }

        seats:
        printf("How many seats per row does your theater have?\n");
        ret = scanf("%d", &r_seats);
        if (!ret) {
            puts("[-]Incompatible type in scanf\n");
            getc(stdin);
            goto seats;
        }
        if (ret == EOF) {
            puts("[-]Scanf failure\n");
            exit(1);
        }

        fprintf(fileB, "%d;%d\n", rows, r_seats);		//Adding rows and seat number to the backup file

        if (fclose(fileB) != 0) {
            puts("[-]Fclose failure (fileB)\n");
            exit(1);
        }
    }
    else {
        puts("[-]Invalid input, try again...");
        goto scanf_backup;
    }

    printf("[+]Total seats: %d\n", rows * r_seats);

    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);				//Binding dinamically to port
    server.sin_addr.s_addr = htonl(INADDR_ANY);			//Binding dinamically to address

    l_socket = socket(AF_INET, SOCK_STREAM, 0);			//Socket creation
    if(l_socket < 0) {
        printf("[-]Error while connecting.\n");
        exit(1);
    }
    printf("[+]Server socket is created.\n");

    const int enable = 1;

    if (setsockopt(l_socket, SOL_SOCKET, SO_REUSEADDR , &enable, sizeof(int)) < 0)	//Option to reuse port
        exit(1);

    if(bind(l_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {		//Binding socket to port
        printf("[-]Port %d binding error.\n", PORT);
        exit(1);
    }
    printf("[+]Bind to port %d.\n", PORT);

    if(listen(l_socket, QUEUE) < 0) {							//Socket waiting for connections
        printf("[-]Listening error.\n");
        exit(1);
    }
    printf("[+]Listening...\n");
    
    struct timeval timeout;
    timeout.tv_sec = 180;
    timeout.tv_usec = 0;

    while(1) {
        c_socket = accept(l_socket, (struct sockaddr *) &client, &addr_size);		//Accepting connection from client
        if(c_socket < 0) {
            printf("[-]Accept failure\n");
            exit(1);
        }
        printf("[+]Connection accepted from %s: %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

	if(setsockopt(c_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		puts("[-]Setsockopt failed.\n");
		exit(1);
	}
        if(pthread_create(&tid, NULL, thread_func, (void *)(long int)c_socket) != 0) {		//Creating threads
            puts("[-]Error during the creation of a thread\n");
            exit(1);
        }
        puts("[+]Thread successfully created\n");
    }
}
