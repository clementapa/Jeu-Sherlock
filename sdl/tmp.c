#include <SDL.h>        
#include <SDL_image.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char ** argv)
{
    	int quit = 0;
    	SDL_Event event;
    	int mx=0,my=0;
 
	// On initialise SDL

    	SDL_Init(SDL_INIT_VIDEO);
 
	// On cree une fenetre graphique de 1024 x 768 pixels
    	SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image",
        	SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
 
	// Dans SDL, on dessine dans ce qui s'appelle un renderer, qui est lié
	// à la fenêtre qu'on vient de créer

    	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

	// On charge ensuite les images bitmap que l'on souhaite afficher dans
	// des SDL_Surface

    	SDL_Surface *bitmap0 = IMG_Load("SH13_0.png");
    	SDL_Surface *bitmap1 = IMG_Load("SH13_1.png");
    	SDL_Surface *bitmap2 = IMG_Load("SH13_2.png");
	SDL_Surface *bitmap3 = IMG_Load("SH13_3.png");
	SDL_Surface *bitmap4 = IMG_Load("SH13_4.png");
	SDL_Surface *bitmap5 = IMG_Load("SH13_5.png");

	// Si on veut, on peut aussi charger des images speciales, qui ont
	// un fond particulier, ici le noir. La fonction SDL_SetColorKey permet
	// de dire que cette couleur noire est transparente, pour afficher
	// correctement le pion par dessus un bitmap
 
    	SDL_Surface *pawn = IMG_Load("pawn.png");
    	SDL_SetColorKey(pawn, SDL_TRUE, SDL_MapRGB(pawn->format, 0, 0, 0 ) );

	// En SDL, une fois qu'on a créé les surfaces, on en dérive les textures
	// Ce sont ces textures qu'on va afficher avec le renderer. Attention,
	// Ici on ne fait que créer les textures. C'est dans la boucle
	// des événements graphiques, un peu plus loin, que l'on va demander
	// l'affichage de ces textures à un emplacement précis de la fenetre

    	SDL_Texture *textureBitmap0 = SDL_CreateTextureFromSurface(renderer, bitmap0);
    	SDL_Texture *textureBitmap1 = SDL_CreateTextureFromSurface(renderer, bitmap1);
    	SDL_Texture *textureBitmap2 = SDL_CreateTextureFromSurface(renderer, bitmap2);
	SDL_Texture *textureBitmap3 = SDL_CreateTextureFromSurface(renderer, bitmap3);
	SDL_Texture *textureBitmap4 = SDL_CreateTextureFromSurface(renderer, bitmap4);
	SDL_Texture *textureBitmap5 = SDL_CreateTextureFromSurface(renderer, bitmap5);
    	SDL_Texture *texturePawn = SDL_CreateTextureFromSurface(renderer, pawn);

	// On détruit les surfaces dont on n'a plus l'utilisation, maintenant
	// que les textures sont prêtes

    	SDL_FreeSurface(bitmap0);
    	SDL_FreeSurface(bitmap1);
    	SDL_FreeSurface(bitmap2);
    	SDL_FreeSurface(bitmap3);
    	SDL_FreeSurface(bitmap4);
    	SDL_FreeSurface(bitmap5);
    	SDL_FreeSurface(pawn);
 
	// Voici la boucle infinie des événements
	// Son fonctionnement est le suivant:
	//	1) regarder si il y a un nouvel événement graphique
	//		si oui, le traiter et mettre à jour l'état du jeu	
	//	2) régénérer les bitmaps avec la fonction SDL_RenderCopy 
    
	while (!quit)
    	{
		// 1) Y'a t'il un nouvel événement graphique
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
					printf("mx=%d my=%d\n",mx,my);
					break;
        		}
		}

		// 2) On redessine. Ce qui est "rendered" en premier
		// est en dessous de ce qui "rendered" en dernier. Dans
		// l'exemple ci-dessous, on affiche d'abord le fond
		// puis le bitmap0, puis le pion et enfin la ligne noire 

		// On efface l'écran avec du gris clair (r=230, g=230, b=230)
        	SDL_SetRenderDrawColor(renderer, 255, 230, 230, 230);
		// on crée un rectangle positionné en 0,0 et de taille 1024x768 
        	SDL_Rect rectBackground = {0, 0, 1024, 768}; 
        	SDL_RenderFillRect(renderer, &rectBackground);

        	SDL_Rect rectBitmap0 = { 0, 0, 500, 330 };
        	SDL_RenderCopy(renderer, textureBitmap0, NULL, &rectBitmap0);
		
		SDL_Rect rectBitmap2 = { 50, 50, 500, 330 };
        	SDL_RenderCopy(renderer, textureBitmap0, NULL, &rectBitmap0);		

        	SDL_Rect rectPawn = { mx, my, 87, 131 };
        	SDL_RenderCopy(renderer, texturePawn, NULL, &rectPawn);

		// SDL_SetRenderDrawColorrenderer, alpha, red, green, blue)
	       	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderDrawLine(renderer, 0,0,500,330);

		// Mettre à jour le contenu de la fenêtre d'un coup grâce au renderer
        	SDL_RenderPresent(renderer);
    	}
 
    	SDL_DestroyTexture(textureBitmap0);
    	SDL_DestroyTexture(textureBitmap1);
    	SDL_DestroyTexture(textureBitmap2);
	SDL_DestroyTexture(textureBitmap3);
	SDL_DestroyTexture(textureBitmap4);
	SDL_DestroyTexture(textureBitmap5);
    	SDL_DestroyTexture(texturePawn);
    	SDL_DestroyRenderer(renderer);
    	SDL_DestroyWindow(window);
 
    	SDL_Quit();
 
    	return 0;
