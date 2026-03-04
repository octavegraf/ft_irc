#pragma once

#define RESET "\033[0m"
#define GREY "\033[38;5;248m"

#define BLACK "\e[0;30m"
#define RED "\e[0;31m" // action execution (examples: read incoming message, execute command)
#define GREEN "\e[0;32m" // incoming text (raw bytes + parsed message)
#define YELLOW "\e[0;33m" // sent message
#define BLUE "\e[0;34m" // Server internal logic
#define PURPLE "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"

// List of colors: https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124
