#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

namespace fs = std::filesystem;

enum class ThemeType
{
    LIGHT = 0,
    BLACK,
};
enum class ViewType
{
    ROW = 0,
    BOX,
    TREE,
};

/// @brief 
/**
 * @class MainWindow
 * @brief Represents the main window of the application.
 *
 * This class manages the main user interface of the application, including drawing elements, handling user input,
 * and managing various settings.
 */
class MainWindow
{
public:
    /**
     * @brief Constructs a new MainWindow object.
     * @param title The title of the main window.
     * @param size The initial size of the main window.
     */
    MainWindow(std::string title = "Example", ImVec2 size = {800, 600});

    /**
     * @brief Destructor for the MainWindow class.
     */
    ~MainWindow();

    /**
     * @brief Enters the main loop of the application.
     * @return An integer status code indicating the result of the main loop.
     */
    int mainloop();

private:
    /**
     * @brief Draws the main window and its contents.
     */
    void draw();

    /**
     * @brief Draws the tree view in the main window.
     * @param size The size of the area to draw the tree view.
     */
    void drawTree(ImVec2 size);

    /**
     * @brief Draws the items view in the main window.
     * @param size The size of the area to draw the items view.
     */
    void drawItems(ImVec2 size);

    /**
     * @brief Draws the title bar of the main window.
     * @param size The size of the area to draw the title bar.
     */
    void drawTitle(ImVec2 size);

    /**
     * @brief Recursively draws a tree structure representing file paths.
     * @param currentPath The current path in the file system to draw.
     */
    void drawRecursiveTree(const fs::path& currentPath);

    /**
     * @brief Displays a popup menu at the current mouse position.
     */
    inline void popupMenu();

    /**
     * @brief Sets up the ImGui style for the main window.
     * @param bStyleDark_ A flag indicating whether to use a dark style.
     * @param alpha_ The transparency level of the style.
     */
    inline void SetupImGuiStyle(bool bStyleDark_, float alpha_);

private:
    sf::RenderWindow* window_ = nullptr; ///< Pointer to the main SFML window.
    sf::Clock deltaClock_;                ///< Clock used for measuring time intervals.
    std::string windowTitle_;             ///< The title of the main window.
    ImVec2 size_;                         ///< The size of the main window.
    int titleHeight_ = 60;                ///< The height of the title bar.
    int treeWidth_ = 300;                 ///< The width of the tree view.
    int itemsWidth_ = 0;                  ///< The width of the items view.
    ImVec2 boxItemSize = {40, 40};        ///< The size of each item box.
    fs::path currentPath_ = fs::current_path(); ///< The current path in the file system.
    fs::path popupMenuPath_ = "";         ///< The path associated with the popup menu.
    std::vector<std::string> pathTmp_;    ///< Temporary storage for file paths.
    ViewType viewType_ = ViewType::ROW;   ///< The type of view for displaying items.
    bool showHidden_ = false;             ///< Flag indicating whether hidden files should be shown.
    bool showSettings_ = false;           ///< Flag indicating whether settings should be displayed.
    ThemeType themeType_ = ThemeType::BLACK; ///< The theme type for the user interface.
    fs::path copyFrom_;                   ///< Path used for file copying.
    bool copyFlag_ = false;               ///< Flag indicating whether copying is in progress.
    char renameBuffer_[1024] = "";        ///< Buffer for renaming files.
    char openBuffer_[1024] = "";          ///< Buffer for opening files.
    std::vector<fs::path> favourites_;    ///< List of favorite file paths.
};



