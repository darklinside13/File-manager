#include <MainWindow.hpp>

MainWindow::MainWindow(std::string title, ImVec2 size):
    windowTitle_(title), size_(size)
{
    window_ = new sf::RenderWindow(sf::VideoMode(size_.x, size_.y), windowTitle_);
    window_->setFramerateLimit(60);
    ImGui::SFML::Init(*window_);
}
MainWindow::~MainWindow()
{
    ImGui::SFML::Shutdown();
    delete window_;
}
int MainWindow::mainloop()
{
    while (window_->isOpen())
    {
        sf::Event event {};
        while (window_->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(*window_, event);

            if (event.type == sf::Event::Closed)
            {
                window_->close();
            }
        }
        ImGui::SFML::Update(*window_, deltaClock_.restart());

        size_ = {(float)window_->getSize().x, (float)window_->getSize().y};

        draw();

        window_->clear();
        ImGui::SFML::Render(*window_);
        window_->display();
    }
    return 0;
}
void MainWindow::draw()
{
    if (ImGui::Begin(windowTitle_.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize))
    {
        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize(size_);

        switch (themeType_)
        {
        case ThemeType::BLACK:
            SetupImGuiStyle(true, 100);
            break;
        case ThemeType::LIGHT:
            SetupImGuiStyle(false, 100);
            break;

        }

        drawTitle({size_.x - 15, (float)titleHeight_});

        ImGui::Columns(2);
        treeWidth_ = ImGui::GetColumnWidth(0);

        drawTree({(float)(treeWidth_ - 15), (float)(size_.y - titleHeight_ - 20)});

        ImGui::NextColumn();

        itemsWidth_ = ImGui::GetColumnWidth(1) - 100;
        drawItems({size_.x - treeWidth_ - 15, size_.y - titleHeight_ - 20});



        ImGui::End();
    }
}
void MainWindow::drawTree(ImVec2 size)
{
    if (ImGui::BeginChild("Tree", size, ImGuiChildFlags_Border))
    {
        for (int i = 0; i < favourites_.size(); i += 1)
        {
            if ( ImGui::Button( favourites_[i].filename().c_str() ) ) currentPath_ = favourites_[i];
            ImGui::SameLine();
            if ( ImGui::Button( "r" ) ) favourites_.erase(favourites_.begin() + i);
        }
        ImGui::Separator();
        ImGui::EndChild();
    }
}
void MainWindow::drawItems(ImVec2 size)
{
    if (ImGui::BeginChild("Items", size, ImGuiChildFlags_Border))
    {
        switch (viewType_)
        {
        case ViewType::ROW:
            for (const auto& item : fs::directory_iterator(currentPath_))
            {
                if (item.path().filename().string()[0] == '.' && !showHidden_) continue;

                if ( ImGui::Button( item.path().filename().c_str() ) )
                {
                    if ( item.is_directory() ) currentPath_ = item.path();
                    else if ( item.is_regular_file() )
                    {
                        
                    }
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    popupMenuPath_ = item;
                    ImGui::OpenPopup("Popup");
                }
                ImGui::NextColumn();
            }
            break;
        case ViewType::BOX:
            if (ImGui::BeginTable("Items", (itemsWidth_ ) / boxItemSize.x))
            {
                for (const auto& item : fs::directory_iterator(currentPath_))
                {
                    if (item.path().filename().string()[0] == '.' && !showHidden_) continue;

                    ImGui::TableNextColumn();
                    if ( ImGui::Button( item.path().filename().c_str(), boxItemSize ) )
                    {
                        if ( item.is_directory() ) currentPath_ = item.path();
                        else if ( item.is_regular_file() ) std::cout<<"File"<<std::endl;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    {
                        popupMenuPath_ = item;
                        ImGui::OpenPopup("Popup");
                    }
                ImGui::NextColumn();
                }
                ImGui::EndTable();
            }
            break;
        case ViewType::TREE:
            drawRecursiveTree(currentPath_);
            break;
        default:
            break;
        }

        popupMenu();

        ImGui::EndChild();
    }
}
void MainWindow::drawTitle(ImVec2 size)
{
    if (ImGui::BeginChild("Title", size, ImGuiChildFlags_Border))
    {
        if (ImGui::Button("<=", {50, 35})) currentPath_ = currentPath_.parent_path();
        ImGui::SameLine();
        if (ImGui::BeginChild("Path", {(ImGui::GetWindowWidth() - 200) > 0 ? (ImGui::GetWindowWidth() - 200) : 1, 35}, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        {
            std::stringstream ss(currentPath_.string());
            std::string s;
            pathTmp_.clear();
            while(getline(ss, s, '/')) if (s != "") pathTmp_.push_back(s);
            if (ImGui::Button("/")) currentPath_ = "/";
            for (int i = 0; i < pathTmp_.size(); i += 1)
            {
                ImGui::SameLine();
                if(ImGui::Button(pathTmp_.at(i).c_str()))
                {
                    fs::path newPath;
                    for (int j = 0; j <= i; j += 1)
                    {
                        newPath += "/";
                        newPath += pathTmp_[j];
                    }
                    newPath += "/";
                    currentPath_ = newPath;
                }
                ImGui::SameLine();
                ImGui::Text("/");
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();
        if (ImGui::Button("Paste", {50,35}))
        {
            if (!copyFrom_.empty())
            {
                fs::copy(copyFrom_, currentPath_);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Sett", {50,35})) showSettings_ = !showSettings_;
        if (showSettings_ && ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoTitleBar))
        {
            showSettings_ = ImGui::IsWindowFocused();
            ImGui::SetWindowPos({size_.x - 220, 20});
            ImGui::SetWindowSize({200, 300});

            ImGui::Separator();
            ImGui::Text("Style");
            if (ImGui::RadioButton("Row", viewType_ == ViewType::ROW)) viewType_ = ViewType::ROW;
            if (ImGui::RadioButton("Box", viewType_ == ViewType::BOX)) viewType_ = ViewType::BOX;
            if (ImGui::RadioButton("Tree", viewType_ == ViewType::TREE)) viewType_ = ViewType::TREE;
            ImGui::Separator();
            ImGui::Text("Theme");
            if (ImGui::RadioButton("Light", themeType_ == ThemeType::LIGHT)) themeType_ = ThemeType::LIGHT;
            if (ImGui::RadioButton("Black", themeType_ == ThemeType::BLACK)) themeType_ = ThemeType::BLACK;
            ImGui::Separator();
            ImGui::Checkbox("Show hidden", &showHidden_);
            ImGui::Separator();
            ImGui::Text("Box item size");
            ImGui::SliderFloat("Width", &boxItemSize.x, 10, 700, "%.0f");
            ImGui::SliderFloat("Height", &boxItemSize.y, 10, 70, "%.0f");
            ImGui::Separator();
            ImGui::End();
        }
        ImGui::EndChild();
    }
}
void MainWindow::drawRecursiveTree(const fs::path& currentPath)
{
    for (const auto& item : fs::directory_iterator(currentPath))
    {
        if (item.is_regular_file()) 
        {
            ImGui::TreePush("tmp");
            ImGui::Text(item.path().filename().c_str());
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                popupMenuPath_ = item;
                ImGui::OpenPopup("Popup");
            }
            auto minPos = ImGui::GetItemRectMin();
            auto maxPos = ImGui::GetItemRectMax();
            auto mousePos = ImGui::GetMousePos();
            if ( minPos.x < mousePos.x && 
                minPos.y < mousePos.y && 
                maxPos.x > mousePos.x && 
                maxPos.y > mousePos.y && 
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                ImGui::OpenPopup("Popup");
            }
            ImGui::NextColumn();
            ImGui::TreePop();
        }
        else if (item.is_directory())
        {
            if (ImGui::TreeNodeEx(item.path().filename().c_str(), ImGuiTreeNodeFlags_None))
            {
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    std::cout<<"CHECK"<<std::endl;
                    ImGui::OpenPopup("Popup");
                }
                auto minPos = ImGui::GetItemRectMin();
                auto maxPos = ImGui::GetItemRectMax();
                auto mousePos = ImGui::GetMousePos();
                if ( minPos.x < mousePos.x && 
                    minPos.y < mousePos.y && 
                    maxPos.x > mousePos.x && 
                    maxPos.y > mousePos.y && 
                    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    currentPath_ = item.path();
                }
                drawRecursiveTree(item);
                ImGui::TreePop();
            }
        } 
    }
}
inline void MainWindow::popupMenu()
{
    if (ImGui::BeginPopup("Popup", ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize))
    {
        ImGui::InputTextWithHint("openBuffer", "Open command", openBuffer_, 1024);
        if (ImGui::Button("Open"))
        {
            system( std::string( std::string(openBuffer_) + " " + popupMenuPath_.string() ).c_str() );
        }
        ImGui::Text(popupMenuPath_.filename().c_str());
        if ( ImGui::Button("Copy") )
        {
            copyFrom_ = popupMenuPath_;
        }
        if ( ImGui::Button("Create copy") )
        {
            try{fs::copy_file(popupMenuPath_, fs::path(popupMenuPath_.string() + "_copy"));}
            catch(...){}
        }
        ImGui::InputTextWithHint("renameBuffer", "New name", renameBuffer_, 1024);
        if ( ImGui::Button("Rename") )
        {
            if (std::string(renameBuffer_) != "")
            {
                fs::rename(popupMenuPath_, fs::path(popupMenuPath_.parent_path().string() + "/" + std::string(renameBuffer_)));
            }
        }
        if ( ImGui::Button("Delete") )
        {
            fs::remove(popupMenuPath_);
        }
        if (ImGui::Button("Add to favourites") && fs::is_directory(popupMenuPath_))
        {
            favourites_.push_back(popupMenuPath_);
        }
        ImGui::EndPopup();
    }
    else
    {
    }
}

inline void MainWindow::SetupImGuiStyle( bool bStyleDark_, float alpha_  )
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
    // style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.30f, 0.30f, 0.30f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    // style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    // style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    // style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    // style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    // style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    // style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    // style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    if( bStyleDark_ )
    {
        for (int i = 0; i <= ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            float H, S, V;
            ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

            if( S < 0.1f )
            {
                V = 1.0f - V;
            }
            ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
            if( col.w < 1.00f )
            {
                col.w *= alpha_;
            }
        }
    }
    else
    {
        for (int i = 0; i <= ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            if( col.w < 1.00f )
            {
                col.x *= alpha_;
                col.y *= alpha_;
                col.z *= alpha_;
                col.w *= alpha_;
            }
        }
    }
}


