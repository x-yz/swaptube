#pragma once

#include "scene.h"
using json = nlohmann::json;

class HeaderScene : public Scene {
public:
    HeaderScene(const json& config, const json& contents);
    Pixels query(int& frames_left) override;
    Scene* createScene(const json& config, const json& scene) override {
        return new HeaderScene(config, scene);
    }
};

HeaderScene::HeaderScene(const json& config, const json& contents) : Scene(config, contents) {}

Pixels HeaderScene::query(int& frames_left) {
    frames_left = scene_duration_frames - time;
    time++;

    string header = contents["header"].get<string>();
    string subheader = contents["subheader"].get<string>();
    Pixels header_pix = eqn_to_pix(latex_text(header), 3);
    Pixels subheader_pix = eqn_to_pix(latex_text(subheader), 2);

    pix.fill(0);
    pix.copy(header_pix, (pix.w - header_pix.w)/2, pix.h/2-100, 1, 1);
    pix.copy(subheader_pix, (pix.w - subheader_pix.w)/2, pix.h/2+50, 1, 1);

    return pix;
}