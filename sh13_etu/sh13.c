#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

pthread_t thread_serveur_tcp_id;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char gbuffer[256];
char messageGagnant[256];
char gServerIpAddress[256];
int gServerPort;
char gClientIpAddress[256];
int gClientPort;
char gName[256];
char gNames[4][256];
int gId;
int joueurSel;
int objetSel;
int guiltSel;
int guiltGuess[13];
int tableCartes[4][8];
int b[3];
int goEnabled;
int connectEnabled;
int joueur,objett,table,temp;//pour stockage scanf de "S" et "O"
int perdants[4]={1,1,1,1};//tout les joueurs ne sont pas eliminé, les cases du tableau corresponds aux id des joueurs (valeur à 1) si un joueur est éliminé avec id sont identif on met perdants[id]=0 pour pouvoir afficher une croix a son prenom
char gagnant[256];//le nom du gagnant pour pouvoir l'afficher à la fin du jeu
char perdant[256];//Le nom du perdant pour pouvoir l'afficher sur le terminale
int id;//stock le joueur courant
int messa,MessageEnable=0;//Pour le chat: MessageEnable pour ouvrir le chatBox
char envoyeur[256];//Nom de la personne qui envoie le message
int guilt;//numero du coupable que l'on recupere via G .... pour pour pouvoir afficher le nom du coupable si la personne à gagné en denoncant le coupable
int SendMessage=-1;//deplier et replier le tableau de message
char *nbobjets[]={"5","5","5","5","4","3","3","3"};
char *nbnoms[]={"Sebastian Moran", "irene Adler", "inspector Lestrade",
  "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
  "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
  "Mrs. Hudson", "Mary Morstan", "James Moriarty"};
char *messages[]={"Vite", "Noob", "mmmmm malin",
     "Vulgarite","c'est parti!!","Tu reves de gagner?","Bien joue","Congrats","Je suis le meilleur","ahahaha","Les MAIN en sortie scolaire"};


volatile int synchro;//piocher direct dans la mémoire

void *fn_serveur_tcp(void *arg)
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd<0)
        {
                printf("sockfd error\n");
                exit(1);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = gClientPort;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
       if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
                printf("bind error\n");
                exit(1);
        }

        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while (1)
        {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0)
                {
                        printf("accept error\n");
                        exit(1);
                }

                bzero(gbuffer,256);
                n = read(newsockfd,gbuffer,255);
                if (n < 0)
                {
                        printf("read error\n");
                        exit(1);
                }
                //printf("%s",gbuffer);

                pthread_mutex_lock( &mutex );
                synchro=1;
                pthread_mutex_unlock( &mutex );

                while (synchro);

     }
}

void sendMessageToServer(char *ipAddress, int portno, char *mess)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char sendbuffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(ipAddress);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("ERROR connecting\n");
                exit(1);
        }

        sprintf(sendbuffer,"%s\n",mess);
        n = write(sockfd,sendbuffer,strlen(sendbuffer));

    close(sockfd);
}

int main(int argc, char ** argv)
{
	int ret;
	int i,j;

    int quit = 0;
    SDL_Event event;
	int mx,my;
	char sendBuffer[256];
	char lname[256];
	int id;

        if (argc<6)
        {
                printf("<app> <Main server ip address> <Main server port> <Client ip address> <Client port> <player name>\n");
                exit(1);
        }

        strcpy(gServerIpAddress,argv[1]);
        gServerPort=atoi(argv[2]);
        strcpy(gClientIpAddress,argv[3]);
        gClientPort=atoi(argv[4]);
        strcpy(gName,argv[5]);

    SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

    SDL_Window * window = SDL_CreateWindow("SDL2 SH13",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Surface *deck[13],*objet[8],*gobutton,*connectbutton,*winner,*fond,*croix,*mess,*PhotoChat[12];

	deck[0] = IMG_Load("media/SH13_0.png");
	deck[1] = IMG_Load("media/SH13_1.png");
	deck[2] = IMG_Load("media/SH13_2.png");
	deck[3] = IMG_Load("media/SH13_3.png");
	deck[4] = IMG_Load("media/SH13_4.png");
	deck[5] = IMG_Load("media/SH13_5.png");
	deck[6] = IMG_Load("media/SH13_6.png");
	deck[7] = IMG_Load("media/SH13_7.png");
	deck[8] = IMG_Load("media/SH13_8.png");
	deck[9] = IMG_Load("media/SH13_9.png");
	deck[10] = IMG_Load("media/SH13_10.png");
	deck[11] = IMG_Load("media/SH13_11.png");
	deck[12] = IMG_Load("media/SH13_12.png");

	objet[0] = IMG_Load("media/SH13_pipe_120x120.png");
	objet[1] = IMG_Load("media/SH13_ampoul1_120x120.png");
	objet[2] = IMG_Load("media/SH13_poing_120x120.png");
	objet[3] = IMG_Load("media/SH13_couronne_120x120.png");
	objet[4] = IMG_Load("media/SH13_carnet_120x120.png");
	objet[5] = IMG_Load("media/SH13_collier_120x120.png");
	objet[6] = IMG_Load("media/SH13_oeil_120x120.png");
	objet[7] = IMG_Load("media/SH13_crane_120x120.png");

  /* Les Photos qui correspondent au messages*/
  PhotoChat[0] = IMG_Load("media/mess1.jpg");
	PhotoChat[1] = IMG_Load("media/mess2.jpg");
	PhotoChat[2] = IMG_Load("media/mess3.jpg");
	PhotoChat[3] = IMG_Load("media/mess4.jpg");
	PhotoChat[4] = IMG_Load("media/mess5.jpg");
	PhotoChat[5] = IMG_Load("media/mess6.jpg");
  PhotoChat[6] = IMG_Load("media/mess7.jpg");
  PhotoChat[7] = IMG_Load("media/mess8.jpg");
  PhotoChat[8] = IMG_Load("media/mess9.jpg");
  PhotoChat[9] = IMG_Load("media/mess11.jpg");
  PhotoChat[10] = IMG_Load("media/mess12.jpg");
  PhotoChat[11] = IMG_Load("media/mess12.jpg");


	gobutton = IMG_Load("media/glass.png");
	connectbutton = IMG_Load("media/connect.jpeg");

  winner = IMG_Load("media/winner.jpg");
  fond = IMG_Load("media/wall.jpg");
  croix = IMG_Load("media/croix.png");
  mess = IMG_Load("media/Message.png");

	strcpy(gNames[0],"-");
	strcpy(gNames[1],"-");
	strcpy(gNames[2],"-");
	strcpy(gNames[3],"-");

	joueurSel=-1;
	objetSel=-1;
	guiltSel=-1;
  guilt=100;

	b[0]=-1;
	b[1]=-1;
	b[2]=-1;

	for (i=0;i<13;i++)
		guiltGuess[i]=0;

	for (i=0;i<4;i++)
		for (j=0;j<8;j++)
			tableCartes[i][j]=-1;

	goEnabled=0;
	connectEnabled=1;

    SDL_Texture *texture_PhotoChat[12],*texture_mes,*texture_croix,*texture_fond,*texture_deck[13],*texture_gobutton,*texture_connectbutton,*texture_objet[8],*texture_winner;

	for (i=0;i<13;i++)
		texture_deck[i] = SDL_CreateTextureFromSurface(renderer, deck[i]);
	for (i=0;i<8;i++)
		texture_objet[i] = SDL_CreateTextureFromSurface(renderer, objet[i]);
  for (i=0;i<11;i++)
  	texture_PhotoChat[i] = SDL_CreateTextureFromSurface(renderer, PhotoChat[i]);

    texture_gobutton = SDL_CreateTextureFromSurface(renderer, gobutton);
    texture_connectbutton = SDL_CreateTextureFromSurface(renderer, connectbutton);
    texture_winner = SDL_CreateTextureFromSurface(renderer, winner);

    texture_fond = SDL_CreateTextureFromSurface(renderer, fond);
    texture_croix = SDL_CreateTextureFromSurface(renderer, croix);
    texture_mes = SDL_CreateTextureFromSurface(renderer, mess);

    TTF_Font* Sans = TTF_OpenFont("media/AppleGaramond-BoldItalic.ttf", 18);
    printf("Sans=%p\n",Sans);

   /* Creation du thread serveur tcp. */
   printf ("Creation du thread serveur tcp !\n");
   synchro=0;
   ret = pthread_create ( & thread_serveur_tcp_id, NULL, fn_serveur_tcp, NULL);

    while (!quit)
    {
	if (SDL_PollEvent(&event))
	{
		//printf("un event\n");
        	switch (event.type)
        	{
            		case SDL_QUIT:
                		quit = 1;
                		break;
			case  SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState( &mx, &my );
				//printf("mx=%d my=%d\n",mx,my);
				if ((mx<200) && (my<50) && (connectEnabled==1))
				{
					sprintf(sendBuffer,"C %s %d %s",gClientIpAddress,gClientPort,gName);
          sendMessageToServer(gServerIpAddress,gServerPort, sendBuffer);
					connectEnabled=0;
				}
				else if ((mx>=0) && (mx<200) && (my>=90) && (my<330))
				{
					joueurSel=(my-90)/60;
					guiltSel=-1;
				}
				else if ((mx>=200) && (mx<680) && (my>=0) && (my<90))
				{
					objetSel=(mx-200)/60;
					guiltSel=-1;
				}
				else if ((mx>=100) && (mx<250) && (my>=350) && (my<740))
				{
					joueurSel=-1;
					objetSel=-1;
					guiltSel=(my-350)/30;
				}
				else if ((mx>=250) && (mx<300) && (my>=350) && (my<740))
				{
					int ind=(my-350)/30;
					guiltGuess[ind]=1-guiltGuess[ind];

				}
        else if ((mx>=305) && (mx<345)&& (my>=350) && (my<390))
        {
          SendMessage*=-1;
        }
        /* Envoie Message*/
        else if ((mx>=305) && (mx<605) && (my>=410) && (my<740) && SendMessage==1)
        {
          int Mg=(my-410)/30;
          SendMessage*=-1;
          sprintf(sendBuffer,"Z %d %d",gId, Mg);
          sendMessageToServer(gServerIpAddress,gServerPort, sendBuffer);
        }


				else if ((mx>=500) && (mx<700) && (my>=350) && (my<450) && (goEnabled==1))
				{
					printf("go! joueur=%d objet=%d guilt=%d\n",joueurSel, objetSel, guiltSel);
					if (guiltSel!=-1)
					{
						sprintf(sendBuffer,"G %d %d",gId, guiltSel);
            sendMessageToServer(gServerIpAddress,gServerPort, sendBuffer);

					}
					else if ((objetSel!=-1) && (joueurSel==-1))
					{
						sprintf(sendBuffer,"O %d %d",gId, objetSel);
            sendMessageToServer(gServerIpAddress,gServerPort, sendBuffer);

					}
					else if ((objetSel!=-1) && (joueurSel!=-1))
					{
						sprintf(sendBuffer,"S %d %d %d",gId, joueurSel,objetSel);
            sendMessageToServer(gServerIpAddress,gServerPort, sendBuffer);

					}
				}
				else
				{
          SendMessage=-1;
					joueurSel=-1;
					objetSel=-1;
					guiltSel=-1;
				}
				break;
			case  SDL_MOUSEMOTION:
				SDL_GetMouseState( &mx, &my );
				break;
        	}
	}

        if (synchro==1)
        {
                pthread_mutex_lock( &mutex );
                printf("consomme |%s|\n",gbuffer);
		switch (gbuffer[0])
		{
			// Message 'I' : le joueur recoit son Id
			case 'I':
				// RAJOUTER DU CODE ICI
        gId=atoi(&gbuffer[2]);
				break;
			// Message 'L' : le joueur recoit la liste des joueurs
			case 'L':
				// RAJOUTER DU CODE ICI
        sscanf( gbuffer, "L %s %s %s %s", gNames[0],gNames[1],gNames[2],gNames[3]);
				break;
			// Message 'D' : le joueur recoit ses trois cartes
			case 'D':
				sscanf( gbuffer, "D %d %d %d", b,b+1,b+2);
				break;
			// Message 'M' : le joueur recoit le n° du joueur courant
			// Cela permet d'affecter goEnabled pour autoriser l'affichage du bouton go
			case 'M':
        sscanf( gbuffer, "M %d ", &id);
        if(id==gId && guilt==100)
        goEnabled=1;
        else
        goEnabled=0;
				break;
			// Message 'V' : le joueur recoit une valeur de tableCartes
			case 'V':
        sscanf(gbuffer,"V %d %d %d %d %d %d %d %d",tableCartes[gId],tableCartes[gId]+1,tableCartes[gId]+2,tableCartes[gId]+3,tableCartes[gId]+4,tableCartes[gId]+5,tableCartes[gId]+6,tableCartes[gId]+7);
				break;
      case 'S':
        sscanf(gbuffer,"S %d %d %d",&table,&joueur,&objett);
        tableCartes[joueur][objett]=table;
        break;
      case 'O':
        sscanf(gbuffer,"O %d %d %d",&joueur,&objett,&temp);
        if(joueur!=gId && tableCartes[joueur][objett]==-1)
        tableCartes[joueur][objett]=temp;
        break;
      case 'G':
        sscanf(gbuffer,"G %s %d a gagné!!!!",gagnant,&guilt);
        goEnabled=0;
      break;
      case 'E':
        sscanf(gbuffer,"E %s %d a tenté sa chance mais est éliminé!!!!!",perdant,&id);
        perdants[id]=0;
      break;
      case 'Z':
        sscanf(gbuffer,"Z %s %d ",envoyeur,&messa);
        MessageEnable=1;
      break;
		}
		synchro=0;
                pthread_mutex_unlock( &mutex );
        }

        SDL_Rect dstrect_grille = { 512-250, 10, 500, 350 };
        SDL_Rect dstrect_image = { 0, 0, 500, 330 };
        SDL_Rect dstrect_image1 = { 0, 340, 250, 330/2 };

        SDL_Rect dstrect_fond = { 0, 0, 1024, 768};
        SDL_Rect dstrect_mes = { 305, 350, 40, 40};
  SDL_RenderCopy(renderer, texture_fond, NULL, &dstrect_fond);
  SDL_RenderCopy(renderer, texture_mes, NULL, &dstrect_mes);
  //SDL_SetRenderDrawColor(renderer, 255, 230, 230, 230);
	//SDL_Rect rect = {0, 0, 1024, 768};
	//SDL_RenderFillRect(renderer, &rect);

	if (joueurSel!=-1)
	{
		SDL_SetRenderDrawColor(renderer, 255, 180, 180, 255);
		SDL_Rect rect1 = {0, 90+joueurSel*60, 200 , 60};
		SDL_RenderFillRect(renderer, &rect1);
	}

	if (objetSel!=-1)
	{
		SDL_SetRenderDrawColor(renderer, 180, 255, 180, 255);
		SDL_Rect rect1 = {200+objetSel*60, 0, 60 , 90};
		SDL_RenderFillRect(renderer, &rect1);
	}

	if (guiltSel!=-1)
	{
		SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);
		SDL_Rect rect1 = {100, 350+guiltSel*30, 150 , 30};
		SDL_RenderFillRect(renderer, &rect1);
	}

  for(int i=0;i<4;i++){
      if(perdants[i]==0){
        SDL_Rect dstrect_croix = { 155, 100+ i*60, 40, 40};
        SDL_RenderCopy(renderer, texture_croix, NULL, &dstrect_croix);
      }
  }

	{
        SDL_Rect dstrect_pipe = { 210, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
        SDL_Rect dstrect_ampoule = { 270, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
        SDL_Rect dstrect_poing = { 330, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
        SDL_Rect dstrect_couronne = { 390, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
        SDL_Rect dstrect_carnet = { 450, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
        SDL_Rect dstrect_collier = { 510, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[5], NULL, &dstrect_collier);
        SDL_Rect dstrect_oeil = { 570, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[6], NULL, &dstrect_oeil);
        SDL_Rect dstrect_crane = { 630, 10, 40, 40 };
        SDL_RenderCopy(renderer, texture_objet[7], NULL, &dstrect_crane);
	}
        SDL_Color col1 = {0, 0, 0};
        for (i=0;i<8;i++)
        {
                SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, nbobjets[i], col1);
                SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

                SDL_Rect Message_rect; //create a rect
                Message_rect.x = 230+i*60;  //controls the rect's x coordinate
                Message_rect.y = 50; // controls the rect's y coordinte
                Message_rect.w = surfaceMessage->w; // controls the width of the rect
                Message_rect.h = surfaceMessage->h; // controls the height of the rect

                SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                SDL_DestroyTexture(Message);
                SDL_FreeSurface(surfaceMessage);
        }

        for (i=0;i<13;i++)
        {
                SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, nbnoms[i], col1);
                SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

                SDL_Rect Message_rect;
                Message_rect.x = 105;
                Message_rect.y = 350+i*30;
                Message_rect.w = surfaceMessage->w;
                Message_rect.h = surfaceMessage->h;

                SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                SDL_DestroyTexture(Message);
                SDL_FreeSurface(surfaceMessage);
        }

        SDL_Color col2 = {0, 0, 0};
          if(SendMessage==1){
            for (i=0;i<13;i++)
              SDL_RenderDrawLine(renderer, 310,350+60+i*30,510,350+60+i*30);
            SDL_RenderDrawLine(renderer, 310,350+60,310,740);
            SDL_RenderDrawLine(renderer, 310+200,350+60,310+200,740);

            for (i=0;i<11;i++)
            {
                    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, messages[i], col2);
                    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

                    SDL_Rect Message_rect;
                    Message_rect.x = 315;
                    Message_rect.y = 410+i*30;
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;

                    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                    SDL_DestroyTexture(Message);
                    SDL_FreeSurface(surfaceMessage);
            }

          }

  /*afficher le chat*/
  if (MessageEnable==1) {

    /*afficher chat*/
    sprintf(messageGagnant," Chat Box :");
    SDL_Surface* surfaceMessage1 = TTF_RenderText_Solid(Sans,messageGagnant,col2);
    SDL_Texture* Message1 = SDL_CreateTextureFromSurface(renderer, surfaceMessage1);

    SDL_Rect Message_rect1; //create a rect
    Message_rect1.x = 750;  //controls the rect's x coordinate
    Message_rect1.y = 360+180-5; // controls the rect's y coordinte
    Message_rect1.w = surfaceMessage1->w; // controls the width of the rect
    Message_rect1.h = surfaceMessage1->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message1, NULL, &Message_rect1);
    SDL_DestroyTexture(Message1);
    SDL_FreeSurface(surfaceMessage1);
    /*************************/

    /*afficher photo au dessous*/
    SDL_Rect dstrect = { 750, 360+180+40, 1000/4, 660/4 };
    SDL_RenderCopy(renderer, texture_PhotoChat[messa], NULL, &dstrect);

    sprintf(messageGagnant," %s : %s",envoyeur,messages[messa]);
    SDL_Surface* surfaceMessage2 = TTF_RenderText_Solid(Sans,messageGagnant,col2);
    SDL_Texture* Message2 = SDL_CreateTextureFromSurface(renderer, surfaceMessage2);

    SDL_Rect Message_rect2; //create a rect
    Message_rect2.x = 750;  //controls the rect's x coordinate
    Message_rect2.y = 360+180+15; // controls the rect's y coordinte
    Message_rect2.w = surfaceMessage2->w; // controls the width of the rect
    Message_rect2.h = surfaceMessage2->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message2, NULL, &Message_rect2);
    SDL_DestroyTexture(Message2);
    SDL_FreeSurface(surfaceMessage2);
    /***********************/
  }


	for (i=0;i<4;i++)
        	for (j=0;j<8;j++)
        	{
			if (tableCartes[i][j]!=-1)
			{
				char mess[10];
				if (tableCartes[i][j]==100)
					sprintf(mess,"*");
				else
					sprintf(mess,"%d",tableCartes[i][j]);
                		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, mess, col1);
                		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

                		SDL_Rect Message_rect;
                		Message_rect.x = 230+j*60;
                		Message_rect.y = 110+i*60;
                		Message_rect.w = surfaceMessage->w;
                		Message_rect.h = surfaceMessage->h;

                		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                		SDL_DestroyTexture(Message);
                		SDL_FreeSurface(surfaceMessage);
			}
        	}

	// Sebastian Moran
	{
        SDL_Rect dstrect_crane = { 0, 350, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[7], NULL, &dstrect_crane);
	}
	{
        SDL_Rect dstrect_poing = { 30, 350, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
	}
	// Irene Adler
	{
        SDL_Rect dstrect_crane = { 0, 380, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[7], NULL, &dstrect_crane);
	}
	{
        SDL_Rect dstrect_ampoule = { 30, 380, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
	}
	{
        SDL_Rect dstrect_collier = { 60, 380, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[5], NULL, &dstrect_collier);
	}
	// Inspector Lestrade
	{
        SDL_Rect dstrect_couronne = { 0, 410, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
	}
	{
        SDL_Rect dstrect_oeil = { 30, 410, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[6], NULL, &dstrect_oeil);
	}
	{
        SDL_Rect dstrect_carnet = { 60, 410, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
	}
	// Inspector Gregson
	{
        SDL_Rect dstrect_couronne = { 0, 440, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
	}
	{
        SDL_Rect dstrect_poing = { 30, 440, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
	}
	{
        SDL_Rect dstrect_carnet = { 60, 440, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
	}
	// Inspector Baynes
	{
        SDL_Rect dstrect_couronne = { 0, 470, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
	}
	{
        SDL_Rect dstrect_ampoule = { 30, 470, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
	}
	// Inspector Bradstreet
	{
        SDL_Rect dstrect_couronne = { 0, 500, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
	}
	{
        SDL_Rect dstrect_poing = { 30, 500, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
	}
	// Inspector Hopkins
	{
        SDL_Rect dstrect_couronne = { 0, 530, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[3], NULL, &dstrect_couronne);
	}
	{
        SDL_Rect dstrect_pipe = { 30, 530, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
	}
	{
        SDL_Rect dstrect_oeil = { 60, 530, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[6], NULL, &dstrect_oeil);
	}
	// Sherlock Holmes
	{
        SDL_Rect dstrect_pipe = { 0, 560, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
	}
	{
        SDL_Rect dstrect_ampoule = { 30, 560, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
	}
	{
        SDL_Rect dstrect_poing = { 60, 560, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
	}
	// John Watson
	{
        SDL_Rect dstrect_pipe = { 0, 590, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
	}
	{
        SDL_Rect dstrect_oeil = { 30, 590, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[6], NULL, &dstrect_oeil);
	}
	{
        SDL_Rect dstrect_poing = { 60, 590, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[2], NULL, &dstrect_poing);
	}
	// Mycroft Holmes
	{
        SDL_Rect dstrect_pipe = { 0, 620, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
	}
	{
        SDL_Rect dstrect_ampoule = { 30, 620, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
	}
	{
        SDL_Rect dstrect_carnet = { 60, 620, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
	}
	// Mrs. Hudson
	{
        SDL_Rect dstrect_pipe = { 0, 650, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[0], NULL, &dstrect_pipe);
	}
	{
        SDL_Rect dstrect_collier = { 30, 650, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[5], NULL, &dstrect_collier);
	}
	// Mary Morstan
	{
        SDL_Rect dstrect_carnet = { 0, 680, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[4], NULL, &dstrect_carnet);
	}
	{
        SDL_Rect dstrect_collier = { 30, 680, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[5], NULL, &dstrect_collier);
	}
	// James Moriarty
	{
        SDL_Rect dstrect_crane = { 0, 710, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[7], NULL, &dstrect_crane);
	}
	{
        SDL_Rect dstrect_ampoule = { 30, 710, 30, 30 };
        SDL_RenderCopy(renderer, texture_objet[1], NULL, &dstrect_ampoule);
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	// Afficher les suppositions
	for (i=0;i<13;i++)
		if (guiltGuess[i])
		{
			SDL_RenderDrawLine(renderer, 250,350+i*30,300,380+i*30);
			SDL_RenderDrawLine(renderer, 250,380+i*30,300,350+i*30);
		}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawLine(renderer, 0,30+60,680,30+60);
	SDL_RenderDrawLine(renderer, 0,30+120,680,30+120);
	SDL_RenderDrawLine(renderer, 0,30+180,680,30+180);
	SDL_RenderDrawLine(renderer, 0,30+240,680,30+240);
	SDL_RenderDrawLine(renderer, 0,30+300,680,30+300);

	SDL_RenderDrawLine(renderer, 200,0,200,330);
	SDL_RenderDrawLine(renderer, 260,0,260,330);
	SDL_RenderDrawLine(renderer, 320,0,320,330);
	SDL_RenderDrawLine(renderer, 380,0,380,330);
	SDL_RenderDrawLine(renderer, 440,0,440,330);
	SDL_RenderDrawLine(renderer, 500,0,500,330);
	SDL_RenderDrawLine(renderer, 560,0,560,330);
	SDL_RenderDrawLine(renderer, 620,0,620,330);
	SDL_RenderDrawLine(renderer, 680,0,680,330);

	for (i=0;i<14;i++)
		SDL_RenderDrawLine(renderer, 0,350+i*30,300,350+i*30);
	SDL_RenderDrawLine(renderer, 100,350,100,740);
	SDL_RenderDrawLine(renderer, 250,350,250,740);
	SDL_RenderDrawLine(renderer, 300,350,300,740);

        //SDL_RenderCopy(renderer, texture_grille, NULL, &dstrect_grille);
	if (b[0]!=-1)
	{
        	SDL_Rect dstrect = { 750, 0, 1000/4, 660/4 };
        	SDL_RenderCopy(renderer, texture_deck[b[0]], NULL, &dstrect);
	}
	if (b[1]!=-1)
	{
        	SDL_Rect dstrect = { 750, 180, 1000/4, 660/4 };
        	SDL_RenderCopy(renderer, texture_deck[b[1]], NULL, &dstrect);
	}
	if (b[2]!=-1)
	{
        	SDL_Rect dstrect = { 750, 360, 1000/4, 660/4 };
        	SDL_RenderCopy(renderer, texture_deck[b[2]], NULL, &dstrect);
	}

	// Le bouton go
	if (goEnabled==1)
	{
        	SDL_Rect dstrect = { 500, 350, 200, 150 };
        	SDL_RenderCopy(renderer, texture_gobutton, NULL, &dstrect);
	}
	// Le bouton connect
	if (connectEnabled==1)
	{
        	SDL_Rect dstrect = { 0, 0, 50, 50 };
        	SDL_RenderCopy(renderer, texture_connectbutton, NULL, &dstrect);
	}

  if(guilt!=100){
    SDL_Rect dstrect = {350 , 400, 1000/3,660/3 };
    SDL_RenderCopy(renderer, texture_winner, NULL, &dstrect);
    SDL_Color col ={0,0,0};
    sprintf(messageGagnant,"Felicitations a %s qui est notre gagnant!!",gagnant);
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans,messageGagnant,col);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 350;  //controls the rect's x coordinate
    Message_rect.y = 650; // controls the rect's y coordinte
    Message_rect.w = surfaceMessage->w; // controls the width of the rect
    Message_rect.h = surfaceMessage->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);

    sprintf(messageGagnant,"Le coupable etait %s !!!",nbnoms[guilt]);
    SDL_Surface* surfaceMessage2 = TTF_RenderText_Solid(Sans,messageGagnant,col);
    SDL_Texture* Message2 = SDL_CreateTextureFromSurface(renderer, surfaceMessage2);

    SDL_Rect Message_rect2; //create a rect
    Message_rect2.x = 350;  //controls the rect's x coordinate
    Message_rect2.y = 680; // controls the rect's y coordinte
    Message_rect2.w = surfaceMessage2->w; // controls the width of the rect
    Message_rect2.h = surfaceMessage2->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message2, NULL, &Message_rect2);
    SDL_DestroyTexture(Message2);
    SDL_FreeSurface(surfaceMessage2);

    sprintf(messageGagnant,"Vous pouvez relancer une partie si vous le souhaitez !!!");
    SDL_Surface* surfaceMessage1 = TTF_RenderText_Solid(Sans,messageGagnant,col);
    SDL_Texture* Message1 = SDL_CreateTextureFromSurface(renderer, surfaceMessage1);

    SDL_Rect Message_rect1; //create a rect
    Message_rect1.x = 350;  //controls the rect's x coordinate
    Message_rect1.y = 700; // controls the rect's y coordinte
    Message_rect1.w = surfaceMessage1->w; // controls the width of the rect
    Message_rect1.h = surfaceMessage1->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message1, NULL, &Message_rect1);
    SDL_DestroyTexture(Message1);
    SDL_FreeSurface(surfaceMessage1);
  }

        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        //SDL_RenderDrawLine(renderer, 0, 0, 200, 200);

	SDL_Color col = {0, 0, 0};
	for (i=0;i<4;i++)
		if (strlen(gNames[i])>0)
		{
		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, gNames[i], col);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

		SDL_Rect Message_rect; //create a rect
		Message_rect.x = 10;  //controls the rect's x coordinate
		Message_rect.y = 110+i*60; // controls the rect's y coordinte
		Message_rect.w = surfaceMessage->w; // controls the width of the rect
		Message_rect.h = surfaceMessage->h; // controls the height of the rect

		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    		SDL_DestroyTexture(Message);
    		SDL_FreeSurface(surfaceMessage);
		}

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture_deck[0]);
    SDL_DestroyTexture(texture_deck[1]);
    SDL_FreeSurface(deck[0]);
    SDL_FreeSurface(deck[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
