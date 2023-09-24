#pragma once

#include "scene.cpp"

class HeaderScene : public Scene {
public:
    Scene* createScene(const int width, const int height) override {
        return new HeaderScene(width, height);
    }

    HeaderScene(const int width, const int height) : Scene(width, height) {
        string header = header;
        string subheader = subheader;
        Pixels header_pix = eqn_to_pix(latex_text(header), pix.w / 640 + 1);
        Pixels subheader_pix = eqn_to_pix(latex_text(subheader), pix.w / 640);

        pix.fill(BLACK);
        pix.copy(header_pix, (pix.w - header_pix.w)/2, pix.h/2-100, 1);
        pix.copy(subheader_pix, (pix.w - subheader_pix.w)/2, pix.h/2+50, 1);
    }

    const Pixels& query(bool& done_scene) {
        done_scene = time >= scene_duration_frames;

        Pixels ret(pix.w, pix.h);
        ret.fill(BLACK);
        time++;
        int fade_in_or_out_frames = min(scene_duration_frames/3., 3.*VIDEO_FRAMERATE);
        int second_thirdile = scene_duration_frames - fade_in_or_out_frames;
        if(time < fade_in_or_out_frames){
            ret.copy(pix, 0, 0, smoother2(static_cast<double>(time) / fade_in_or_out_frames));
            return ret;
        }
        else if(time >= second_thirdile){
            ret.copy(pix, 0, 0, smoother2(static_cast<double>(scene_duration_frames - time) / fade_in_or_out_frames));
            return ret;
        }
        else return pix;
    }
private:
    string header;
    string subheader;
};
