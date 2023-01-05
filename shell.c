#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <termios.h>


struct sigaction action;
pid_t p;

int parse_line(char*input,char **argv[]) // on donne a cette fonction une chaine et un table de chaine en entrer 
{                                  
    char*c=NULL;
    char*v=NULL;
    char* s = strdup(input);
    c = strsep (&s," ");
    if(s==NULL){
        v = strsep (&c,"=");
        if(c!=NULL && v != NULL){
            setenv(v,c,0);
            return 0;
        }
    }
	char *found;
	(*argv)=malloc(8);
	char* string = strdup(input);
	int i=0;
        while( (found = strsep(&string," ")) != NULL ){ 
        // a chaque espaces ont  divise " "
		    if(strlen(found)!=0){ 
                char *env=strdup(found)  ;             
                found = strsep(&env,"$");  
                //il ne faut pas qu'il soit une variable d'env
                if(env!=NULL){                  
                    if((char*)getenv(env)!=NULL){
                        found=(char*)getenv(env);
                    }
                }
                (*argv)[i]=malloc(sizeof(found)+1);
		        strcpy((*argv)[i],found);
	            i++;
            }    
            //les null sont supprimer 
	    }
	(*argv)[i]=NULL;                               //la derniere case du tableau est nulle 
	return i;
}
// on prend plusieurs commandes sous formes de tableau et on les excutes
void commande_execution(char*argv[]) 
{ 
    if(argv==NULL) return;
    if(strcmp(argv[0], "cd")==0){ 
        chdir(argv[1]); //cette ligne nous sert a acceder a une directory donner par le user dans l'entrer standard 
        return;
        
    } else {
        if(strcmp(argv[0], "exit")==0){ 
            exit(0);
            // pour terminer le programme on ecrit exit 
        }

        int e,etat_du_fils;
        e=fork(); //l'execution, il nous faut l'etat du fils pour la fonction wait

        if(e==0){
            execvp(argv[0],argv);
        } else {
	        wait(&etat_du_fils);
	        return;
        }
    }
}

void parse_line_redir(char*str,char**argv[],char**input,char**output) 
// cette fonction fraguemente une chaine s en tableauu de chaine argv
{
	char *commande;
	(*input)=NULL;
	(*output)=NULL;
	char* string = strdup(str);
	commande = strsep(&string,"<>");
	
	
	char* f;
        (*argv)=malloc(8);
   
	char* chaine = strdup(commande);
	int i=0;
        while( (f = strsep(&chaine," ")) != NULL ){
		if (strlen(f) != 0){
		(*argv)[i]=malloc(sizeof(f)+1);
		strcpy((*argv)[i],f);
	         i++;
		}
	}
	(*argv)[i]=NULL;	
	if(strchr(str,'>')){
		char* fichier=strdup(string);
		char *final=malloc(sizeof(fichier));
		strcpy(final,fichier);
		while( (f = strsep(&fichier," ")) != NULL ){
			if(strlen(f)!=0){
				
                if (realloc(final,strlen(f)) == NULL) {    
                            return;
                    }
        
				strcpy(final,f);
			    }
	 	}
		(*output)=malloc(strlen(final));
		strcpy((*output),final);
	}else{
		if(strchr(str,'<')){
			char* file=strdup(string);
			char *final=malloc(sizeof(file));
			strcpy(final,file);
		 	while( (f = strsep(&file," ")) != NULL ){
					if(strlen(f)!=0){
						if (realloc(final,strlen(f)) == NULL) {
                            return;
                		}
						strcpy(final,f);
					}
	 			}
			(*input)=malloc(strlen(final));
			strcpy((*input),final);
		}	
	}
}

void commande_rediriger(char*argv[],char*input,char*output) 
// cette fonction affiche les commandes donner et les executes avec redirection 
{   
    int fd;
    if(strcmp(argv[0], "cd")==0){ 
        chdir(argv[1]);
        return;
        //on change de repertoire pour allez au repertoire donner dans l'entrer standard 
    } else {
        if(strcmp(argv[0], "exit")==0){ 
            exit(0);
        }
        // on ecrit exit pour sortir comme dans commande_execution
        int e;                          //executer une commande
        int fils_etat;
        e=fork();
        if(e==0){
            if (input==NULL && output == NULL){
                execvp(argv[0],argv);
            }else{
                if(input!=NULL){
                    int fd = open(input, O_RDONLY);

		            if(fd!=-1){
		                dup2(fd,STDIN_FILENO);
	                    execvp(argv[0],argv);
		            }
                }
                if(output!=NULL){
                    fd = open(output,O_WRONLY|O_CREAT|O_TRUNC,0777);
		            if(fd!=-1){
		                dup2(fd,STDOUT_FILENO);
	                    execvp(argv[0],argv);
		                close(fd);
		            }else{
		                printf("error");	
		            }
                }
            }
        } else {
	        wait(&fils_etat);
	        return;
        }
    }
}
// on divise la chaine str avec des | et des "" et on retourne un fichier d'entrer et de sortie 

void parse_line_pipes(char*str,char***argv[], char **input, char** output) 
{
	(*input)=NULL;
	(*output)=NULL;
	char *f;
	char *(*arg);
	(arg)=malloc(8);
	(*argv)=malloc(8);
	char* string = strdup(str);
	int j=0;

        while( (f = strsep(&string,"|")) != NULL ){
		    (arg)[j]=malloc(sizeof(f)+1);
		    strcpy((arg)[j],f);
	        j++;
	    }

	if(j==1){
	    parse_line_redir(arg[0],&((*argv)[0]),input,output);
	     return;
	}
	int i=0;
	char *entre1;
	char *tmp=NULL;
	char * sortie2;
	while(i<j){
	    if(i==0){                                                          // dans le cas ou il n'y a aucun pipe
		    parse_line_redir(arg[i],&((*argv)[i]),&entre1,&entre1);
	    } else {
		    if(i==(j-1)){
		        parse_line_redir(arg[i],&((*argv)[i]),&sortie2,&sortie2);
		    } else {
		        parse_line_redir(arg[i],&((*argv)[i]),&tmp,&tmp);
		    }
	    }
	    i++;
	}
	if(entre1!=NULL){
	    (*input)=strdup(entre1);
	}
	if(sortie2!=NULL){
	    (*output)=strdup(sortie2);
	}
}

// cette fonction nous affiche une conmmande parsé
void affichage(char* argv[])
{     

	int i=0;
	while(argv[i]!=NULL){
		printf("%s \n",argv[i]);
		i++;
	}
}

void end(){

    kill(p,SIGINT);

}

int main(int argc,char* argv[]) 
{  
	(void)argc; 
	// l'ouverture du fichier se fait en lecture seule
   int fd = open(argv[1], O_RDONLY); 
   action.sa_handler = end;
	sigaction(SIGINT,&action,NULL);
	
	if(fd != -1 ){                    
	    dup2(fd,0);
	    char tampon[BUFSIZ];
	    int n = read (0, tampon, sizeof (tampon));
	    char * str=(char*)malloc(sizeof(char));
	    int t =0,a=1,i=0;
	    
	    while(a == 1 && i< n){
	
	        while(tampon[i]!='\n'){ // tant qu'on a pas de retour a la ligne
	            *(str+t)=tampon[i];
	            t++;
	            str= realloc(str, t+1);
                i++;
	        }
	        str [t]='\0';
            char *(*arg);
            parse_line(str,&arg);
	        commande_execution(arg);
	        t =0;
	        i++;
	    }
	} else {                        // dans le cas ou le fichier n'existe pas
	    close(fd);
	    int n=1;
	    while(n !=0){
	        char buffer[BUFSIZ];
	        getcwd(buffer, BUFSIZ);
	        int cmp=0;
	        while(buffer[cmp]!='\0' && cmp < BUFSIZ){
	            cmp++;
	        }
	        write(0,buffer,cmp);
	        write(0, "$ ", 2);
	        char tampon[BUFSIZ];
	        n = read (0, tampon, sizeof (tampon));
	        char *s= (char*)malloc(n * sizeof(char)+1);
	        strncpy(s, tampon, n-1);
	        s[n-1]='\0';
            char *(*arg);
            parse_line(s,&arg);
            if(arg!=NULL) commande_execution(arg);
            arg=NULL;
	        free(s);
        }
    }
	return 0;
}


