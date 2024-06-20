#include <windows.h>
#include <iostream>
#include <optional>
#include <vector>

// change resolution of a specific display device
bool change_resolution( const std::string& device_name, DWORD width, DWORD height )
{
    DEVMODE dev_mode;
    ZeroMemory( &dev_mode, sizeof( dev_mode ) );
    dev_mode.dmSize = sizeof( dev_mode );
    dev_mode.dmPelsWidth = width;
    dev_mode.dmPelsHeight = height;
    dev_mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    auto result = ChangeDisplaySettingsEx( device_name.c_str(), &dev_mode, nullptr, CDS_TEST, nullptr );
    if ( result != DISP_CHANGE_SUCCESSFUL )
    {
        std::cerr << "Error testing resolution for " << device_name << ": " << result << std::endl;
        return false;
    }

    result = ChangeDisplaySettingsEx( device_name.c_str(), &dev_mode, nullptr, 0, nullptr );
    if ( result != DISP_CHANGE_SUCCESSFUL )
    {
        std::cerr << "Error changing resolution for " << device_name << ": " << result << std::endl;
        return false;
    }

    return true;
}

// get the current screen resolution of a specific display device
std::optional< std::pair< DWORD, DWORD > > get_current_resolution( const std::string& device_name )
{
    DEVMODE dev_mode;
    ZeroMemory( &dev_mode, sizeof( dev_mode ) );
    dev_mode.dmSize = sizeof( dev_mode );
    if ( EnumDisplaySettings( device_name.c_str(), ENUM_CURRENT_SETTINGS, &dev_mode ) )
    {
        return std::make_pair(dev_mode.dmPelsWidth, dev_mode.dmPelsHeight);
    }
    return std::nullopt;
}

// enumerate all display devices
std::vector< std::string > enumerate_display_devices()
{
    std::vector< std::string > device_names;
    DISPLAY_DEVICE display_device;
    ZeroMemory( &display_device, sizeof( display_device ) );
    display_device.cb = sizeof( display_device );
    int device_index = 0;

    while ( EnumDisplayDevices( nullptr, device_index, &display_device, 0 ) )
    {
        if ( display_device.StateFlags & DISPLAY_DEVICE_ACTIVE )
        {
            device_names.emplace_back( display_device.DeviceName );
        }
        device_index++;
    }

    return device_names;
}

int main()
{
    auto device_names = enumerate_display_devices();

    for ( const auto& device_name : device_names )
    {
        auto current_resolution = get_current_resolution( device_name );

        if ( !current_resolution.has_value() )
        {
            std::cerr << "Failed to get the current resolution for " << device_name << std::endl;
            continue;
        }

        DWORD current_width = current_resolution->first;
        DWORD current_height = current_resolution->second;

        std::cout << "Current resolution for " << device_name << ": " << current_width << "x" << current_height << std::endl;

        if ( current_width == 1920 && current_height == 1080 )
        {
            // change to 2160p
            if ( change_resolution( device_name, 3840, 2160 ) )
            {
                std::cout << "Resolution changed to 3840x2160 (2160p) for " << device_name << std::endl;
            }
            else
            {
                std::cerr << "Failed to change to 3840x2160 (2160p) for " << device_name << std::endl;
            }
        }
        else if ( current_width == 3840 && current_height == 2160 )
        {
            // change to 1080p
            if ( change_resolution(device_name, 1920, 1080 ) )
            {
                std::cout << "Resolution changed to 1920x1080 (1080p) for " << device_name << std::endl;
            }
            else
            {
                std::cerr << "Failed to change to 1920x1080 (1080p) for " << device_name << std::endl;
            }
        }
        else
        {
            std::cout << "Current resolution for " << device_name << " is neither 1080p nor 2160p. No change made." << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
