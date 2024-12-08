#ifndef __COLORS_H
#define __COLORS_H

// (Google) Colors in C shell Github =>
// Source (Colors): https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
//Regular text
#define BLK "\e[0;30m" // Usage: <, @, :, > 
#define RED "\e[0;31m"
#define GRN "\e[0;32m" // Usage: userName, systemName, executables (reveal)
#define YEL "\e[0;33m" // Usage: No Matches found!, 
#define BLU "\e[0;34m" // Usage: relative path
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m" // Usage: files (reveal)

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m" // Usage: perror, fprintf(stderr, ...);s
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m" // Usage: Activities

//Reset
#define RESET "\e[0m"
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"

#endif