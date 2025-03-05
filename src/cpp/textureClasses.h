#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

class LTexture
{
    public:
        LTexture();
        ~LTexture();
        bool loadFromFile( std::string path );

        void destroy();
        void render( float x, float y);

        int getWidth();
        int getHeight();
    
        private:
        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;
};

SDL_Window* gWindow
{
    nullptr
};

LTexture gPngTexture;