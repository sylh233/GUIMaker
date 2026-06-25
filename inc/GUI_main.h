#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace GUI {

// 内禀变量，用于注入用户设置
extern double WinWeight, WinHeight, TextRatio, TextRatioC; // 汉字和英文不同
extern std::string path, ttf_path, wav_path;
extern SDL_Renderer *gui_renderer;
extern SDL_Event *gui_event;
// 用于初始化内禀变量
void initVariable(double WinWeight, double WinHeight, double TextRatio,
                  double TextRatioC);
void initPath(std::string Path);
void initSDL(SDL_Renderer *renderer, SDL_Event *event);
void destroySDL();

struct Point { // 用于描述一个点
	double x, y;
};
typedef struct Point Point;

// 生成一个以p为左上角，宽为width的矩形，length为字长，ratio表示字体宽高比，width/lentgh*ratio就是高度
SDL_FRect genRect(GUI::Point p, double width, size_t length,
                  double ratio = TextRatio);
// 与genRect类似，但这里p为矩形中心
SDL_FRect genRectCenter(GUI::Point p, double width, size_t length,
                        double ratio = TextRatio);
// 获取字体材质
SDL_Texture *getFontTex(SDL_Renderer *renderer, std::string fontName,
                        SDL_Color color, std::string text, float size = 20);
// 检测点是否在矩形里（边界不算）
bool InRect(SDL_FRect *rect, GUI::Point point);
// 获取bmp图片材质
SDL_Texture *getBMP(SDL_Renderer *renderer, std::string bmpName);
// 加载wav音频
bool loadWAV(std::string wav_name, SDL_AudioStream *&stream, Uint8 **wav_data,
             Uint32 *wav_len);
// 设置播放设备，将device_id赋值为id
bool addDevice(SDL_AudioDeviceID &device_id,
               SDL_AudioDeviceID id = SDL_OpenAudioDevice(
                   SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
// 绑定设备，将stream绑定到device_id
bool putStream(SDL_AudioDeviceID device_id, SDL_AudioStream *stream,
               Uint8 **wav_data, Uint32 *wav_len);

typedef std::map<std::string, SDL_Texture *> texMap;

void appendTex(texMap *tm, SDL_Texture *tex, std::string name);

void destroyTexMap(texMap *tm);

}; // namespace GUI

#endif
