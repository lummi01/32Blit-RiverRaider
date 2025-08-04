// River Raider
// 2025 M. Gerloff


#include "riverraider.hpp"
#include "assets.hpp"

using namespace blit;

const Size MAP_SIZE(16, 256);

Font font(font4x6);

struct GAME
{
    int map[4096];
    Vec2 map_pos;
    short ani;
};

struct PLAYER
{
    float x;
    float vel;
    short sprite;
    bool shot;
    float fuel;
    Point start;
};

struct SHOT
{
    Vec2 pos;
};

struct OBJECT
{
    short type;
    Point size;
    Vec2 pos;
    short hit;
};

struct PARTICLE
{
	Point pos;
	int alpha;
    float radius;
};

PLAYER p;
GAME game;
TileMap* map;

static std::vector<OBJECT> object;
static std::vector<SHOT> shot;
static std::vector<PARTICLE> particle;

Timer ani_timer;
Timer shot_timer;

Mat3 callback(uint8_t){
    Mat3 transform = Mat3::translation(
        game.map_pos + Vec2(-80, -56));
    return transform;
};

void UpdateAni(Timer &t){
    game.ani == 0? game.ani = 16: game.ani = 0;       
}

void TimerShot(Timer &t){
    p.shot = true;       
}

void NewParticle(Vec2 pos, Point size)
{
    for (short i=0; i<(size.x + size.y); i++){
        PARTICLE par;
        par.pos = Vec2(pos.x, pos.y) + Vec2(rand() %size.x, rand() %size.y);
        par.alpha = 96 + rand() %160;
        par.radius = 0;
        particle.push_back(par);
    }
}

void UpdateParticle()
{
    for(auto par = particle.begin(); par != particle.end();){
        if(par->alpha < 6){
            par = particle.erase(par);
            continue;
        }
        par->pos += Vec2(0, p.vel);
        par->alpha -= 6;
        par->radius += .2f;
        ++par;
    }
}

int MapCollision(Point pos){
    int x = pos.x * .125f;
    int y = pos.y * .125f;
    return game.map[y * 16 + x];
}

bool ObjectCollision(Vec2 pos1, Vec2 pos2, bool shot){
    for(auto o = object.begin(); o != object.end();){
        if((o->pos.y + o->size.y - game.map_pos.y) > -72 && // object in screen
             pos1.x < (o->pos.x + o->size.x) && 
             pos2.x > o->pos.x && 
             pos1.y < (o->pos.y + o->size.y - game.map_pos.y + 65) && 
             pos2.y > (o->pos.y - game.map_pos.y + 65)){
            if(shot == false && o->type == 80){ // fuel filling
                if(p.fuel > 0)
                    p.fuel -= .1f;
                return false;
                break;
            }
            // object hit
            o->hit--;
            NewParticle(Vec2(o->pos.x, o->pos.y - game.map_pos.y + 70), o->size);
            if (o->hit == 0){
                o = object.erase(o);
            }
            return true;
            break;
        }
        ++o;
    }
    return false;
}

void UpdateControl(){
    if (p.shot && buttons & Button::A){
        p.shot = false;
        shot_timer.start();
        SHOT s;
        s.pos = Vec2(p.x + 3, 98);
        shot.push_back(s);
    }
    if (buttons & Button::DPAD_DOWN)
        p.vel = .3f;
    else if (buttons & Button::DPAD_UP)
        p.vel = .5f;
    else 
        p.vel = .4f;
/*
    if (buttons & Button::DPAD_DOWN)
        p.vel = -.2f;
    else if (buttons & Button::DPAD_UP)
        p.vel = .2f;
    else 
        p.vel = 0;
*/
    game.map_pos.y -= p.vel;

    if (buttons & Button::DPAD_LEFT){
        p.x -= .4f;
        p.sprite = 2;
    }
    else if (buttons & Button::DPAD_RIGHT){
        p.x += .4f;
        p.sprite = 3;
    }
    else
        p.sprite = 1;

}

void UpdateShot(){
    for(auto s = shot.begin(); s != shot.end();){
        s->pos.y -= 1;
        if (s->pos.y < 0){
            s = shot.erase(s);
            continue;
        }
        else if(ObjectCollision(s->pos, Vec2(s->pos + Vec2(1, 1)), true)){
            s = shot.erase(s);
            continue;
        }
        ++s;
    }
}

void NewObject(short type, Point size, Point pos, short hits){
    OBJECT o;
    o.type = type;
    o.size = size;
    o.pos = Vec2(pos.x * 8 + 16, pos.y * 8);
    o.hit = hits;
    object.push_back(o);
}

void UpdateObject(){
    for (auto o = object.begin(); o != object.end();){
        if(o->pos.y + o->size.y - game.map_pos.y > -72){
            if(o->pos.y - game.map_pos.y > 47){
                o = object.erase(o);
                continue;
            }
            else if(o->type == 85){ // plane
                o->pos.y += .5f;
            }
            else{
                float x[2]{.1f, .2f};
                if(o->type == 83 || o->type == 84){ // ship, heli to right
                    o->pos.x += x[o->type - 83];
                    if(MapCollision(Point(o->pos.x - 8, o->pos.y + 4)) == 1)
                        o->type += 4;
                }
                else if(o->type == 87 || o->type == 88){ // ship, heli to left
                    o->pos.x -= x[o->type - 87];
                    if(MapCollision(Point(o->pos.x - 17, o->pos.y + 4)) == 1)
                        o->type -= 4;
                }
            }
        }
        ++o;
    }
}

void load_tilemap(){ 
}

void start(){
// Load the map data from the map memory
    TMX *tmx = (TMX *)asset_tilemap;

    for(short y = 0; y < 256; y++){
        for(short x = 0; x < 16; x++){
            if (game.map[y * 16 + x] == 0){
                short tile = tmx->data[y * 16 + x];
                // land
                if(tile >= 32){
                    game.map[y * 16 + x] = 1;
                }
                // bridge
                else if(tile == 21){
                    NewObject(112, Point(32, 8), Point(x, y), 3);
                }
                // fuel
                else if(tile == 12){
                    NewObject(80, Point(8, 16), Point(x, y), 1);
                }
                // plane
                else if(tile == 11){
                    NewObject(85, Point(8, 8), Point(x, y), 1);
                }
                // tank, ship, heli, plane
                else if(tile > 3 && tile < 11){
                    NewObject(tile + 78, Point(8, 8), Point(x, y), 1);
                }
                // water
                else{
                    game.map[y * 16 + x] = 0;
                }
            }
        }
    }            

    game.map_pos = Vec2(64, 1992);
    p.x = 76;
    p.start = Point(p.x, game.map_pos.y);
    p.fuel = 0;
}

// init
void init(){
    blit::set_screen_mode(ScreenMode::lores);

    screen.sprites = Surface::load(spritesheet);

    map = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(16, 256), screen.sprites);

    ani_timer.init(UpdateAni, 50, -1);
    ani_timer.start();

    shot_timer.init(TimerShot, 400, 1);
    shot_timer.start();

    start();
}

// render
void render(uint32_t time_ms){
    screen.alpha = 255;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    screen.alpha = 255;
    screen.mask = nullptr;

    map->draw(&screen, Rect(16, 0, 128, 112), callback); // map

    // Objects
    for (auto &o : object){
        if (o.pos.y + o.size.y - game.map_pos.y > -80){
            if (o.type == 112) // bridge
                screen.sprite(Rect((o.hit - 1) * 4, 7, 4, 2), Point(o.pos.x, o.pos.y - game.map_pos.y + 57));
            else if (o.type == 80) // fuels
                screen.sprite(Rect(0, 5, 1, 2), Point(o.pos.x, o.pos.y - game.map_pos.y + 65));
            else if (o.type == 84 || o.type == 88) // helicopters
                screen.sprite(o.type + game.ani, Point(o.pos.x, o.pos.y - game.map_pos.y + 65));
            else // ships & tanks
                screen.sprite(o.type, Point(o.pos.x, o.pos.y - game.map_pos.y + 65));
        }
    }

    // shots
    screen.pen = Pen(255, 255, 255);
    for (auto &s : shot)
        screen.rectangle(Rect(s.pos.x, s.pos.y, 2, 2));

    // particles
    screen.pen = Pen(200,200,200);
    for(auto &par : particle){
  	    screen.alpha = par.alpha;
        screen.circle(Point(par.pos.x, par.pos.y),par.radius);
   	}
    screen.alpha = 255;

    // player
    screen.sprite(p.sprite, Point(p.x, 98)); 

    screen.pen = Pen(0, 0, 0);
    screen.rectangle(Rect(16, 112, 128, 8));
    screen.pen = Pen(255, 0, 0);
    screen.rectangle(Rect(91 - p.fuel, 114, 2, 5));
    screen.sprite(Rect(0, 9, 3, 1), Point(69, 113)); // fuel
    
    screen.pen = Pen(255, 255, 255);

    screen.text(std::to_string(int(game.map_pos.y)), font, Point(102, 113)); // object.size()
//    screen.watermark();  
}

// update
void update(uint32_t time){
    p.fuel += .008f;
    if(p.fuel > 22)
        start();

    UpdateControl();
    UpdateObject();
    UpdateShot();
    UpdateParticle();

    // player collision
    if (MapCollision(Point(p.x - 16, game.map_pos.y + 34)) == 1 ||
        MapCollision(Point(p.x - 9, game.map_pos.y + 34)) == 1 ||
        ObjectCollision(Vec2(p.x + 3, 100), Vec2(p.x + 4, 105), false) || 
        ObjectCollision(Vec2(p.x + 1, 102), Vec2(p.x + 6, 103), false)){
        object.clear();
        shot.clear();
        particle.clear();
        start();
    }

    if (game.map_pos.y < -32){
        object.clear();
        shot.clear();
        particle.clear();
        start();
    }
}
