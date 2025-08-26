/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "PathProvider.h"
#include "UIStyle.h"
#include <expected>
#include <iostream>

namespace steamrot {
/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
StylesConfigurator::GetAllStyleNames() {

  // get ui_styles path from path provider
  PathProvider path_provider;
  auto ui_styles_path_result = path_provider.GetUIStylesDirectory();

  if (!ui_styles_path_result) {
    return std::unexpected(ui_styles_path_result.error());
  }

  // set up vector to return
  std::vector<std::string> style_names;

  // set up file extension to look for
  std::string file_extension = ".styles.bin";

  // go through the directory and check for files with the .styles.bin extension
  for (const auto &entry :
       std::filesystem::directory_iterator(ui_styles_path_result.value())) {
    std::cout << "Found file: " << entry.path() << std::endl;
    if (entry.is_regular_file() &&
        (entry.path().string().ends_with(file_extension))) {

      std::string entry_path = entry.path().filename().string();
      // remove the suffix from the filename to get the style name
      std::string style_name =
          entry_path.substr(0, entry_path.size() - file_extension.size());
      // add the style name to the vector
      style_names.push_back(style_name);
    }
  }
  return style_names;
}

/////////////////////////////////////////////////
std::expected<UIStyle, FailInfo>
StylesConfigurator::ConfigureStyle(const UIStyleData &style_data) {
  // create UIStyle object to return
  UIStyle ui_style;

  return ui_style;
}

} // namespace steamrot
