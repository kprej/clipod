#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

#include <filesystem>

namespace ui
{
class fileBrowser_t : public ftxui::ComponentBase
{
public:
    ~fileBrowser_t ();
    fileBrowser_t ();

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<std::filesystem::path const &> directorySelected;

private:
    void setup (std::filesystem::path const &path_);
    void onEnter ();

    std::filesystem::path m_currentPath;

    std::vector<std::string> m_directoryStr;
    std::vector<std::filesystem::path> m_directoryPaths;

    int m_selected;
    ftxui::Component m_menu;
};
} // namespace ui
