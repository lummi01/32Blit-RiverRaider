// River Raider
// 2025 M. Gerloff


#include "riverraider.hpp"
#include "assets.hpp"

using namespace blit;

const Size MAP_SIZE(16, 256);

Font font(font4x6);

Vec2 map_pos(64, 1992);

struct GAME
{
    int map[4096];
    short ani = 0;
};

struct PLAYER
{
    float x;
    float vel;
    short sprite;
    bool shot;
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
    Vec2 vel;
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
        map_pos + Vec2(-80, -56));
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


void map_collision(Point pos){
    int x = pos.x * .125f;
    int y = pos.y * .125f;
    if (game.map[y * 16 + x] > 0)
        p.x = 76;
}

bool ObjectShotCollision(Vec2 pos){
    for(auto o = object.begin(); o != object.end();){
        if ((o->pos.y + o->size.y - map_pos.y) > -72 && 
             pos.x < (o->pos.x + o->size.x) && 
             (pos.x + 1) > o->pos.x && 
             pos.y < (o->pos.y + o->size.y - map_pos.y + 65) && 
             (pos.y + 1) > (o->pos.y - map_pos.y + 65)){
            o->hit--;
            NewParticle(Vec2(o->pos.x, o->pos.y - map_pos.y + 70), o->size);
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

void UpdateShot(){
    for(auto s = shot.begin(); s != shot.end();){
        s->pos.y -= 1;
        if (s->pos.y < 0){
            s = shot.erase(s);
            continue;
        }
        else if(ObjectShotCollision(s->pos)){
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
        if (o->pos.y - map_pos.y > 47)
            o = object.erase(o);
        else{
            ++o;
        }
    }
}

// Load the map data from the map memory
void load_tilemap(){ 
    TMX *tmx = (TMX *)asset_tilemap;

    for(short y = 0; y < 256; y++){
        for(short x = 0; x < 16; x++){
            if (game.map[y * 16 + x] == 0){
                short tile = tmx->data[y * 16 + x];
                // land
                if (tile >= 32){
                    game.map[y * 16 + x] = 1;
                }
                // bridge
                else if (tile == 21){
                    NewObject(112, Point(32, 8), Point(x, y), 3);
                }
                // fuel
                else if (tile == 11){
                    NewObject(80, Point(8, 16), Point(x, y), 1);
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

    load_tilemap();
    p.x = 98;
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
        if (o.pos.y + o.size.y - map_pos.y > -80){
            if (o.type == 112) // bridge
                screen.sprite(Rect((o.hit - 1) * 4, 7, 4, 2), Point(o.pos.x, o.pos.y - map_pos.y + 57));
            else if (o.type == 80) // fuels
                screen.sprite(Rect(0, 5, 1, 2), Point(o.pos.x, o.pos.y - map_pos.y + 65));
            else if (o.type == 84 || o.type == 88) // helicopters
                screen.sprite(o.type + game.ani, Point(o.pos.x, o.pos.y - map_pos.y + 65));
            else // ships & tanks
                screen.sprite(o.type, Point(o.pos.x, o.pos.y - map_pos.y + 65));
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
    screen.sprite(Rect(13, 0, 3, 1), Point(68, 113)); // fuel
    screen.pen = Pen(255, 255, 255);

    screen.text(std::to_string(int(map_pos.y)), font, Point(102, 113)); // object.size()
//    screen.watermark();  
}

// update
void update(uint32_t time){
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

    map_pos.y -= p.vel;

    if (buttons & Button::DPAD_LEFT)
    {
        p.x -= .4f;
        p.sprite = 2;
    }
    else if (buttons & Button::DPAD_RIGHT)
    {
        p.x += .4f;
        p.sprite = 3;
    }
    else
        p.sprite = 1;

    UpdateObject();
    UpdateShot();
    UpdateParticle();

    map_collision(Point(p.x - 16, map_pos.y + 32));

    if (map_pos.y < -32)
      std::exit(3);
}
