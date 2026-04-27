## [Unreleased] - 2026-XX-XX
### Changed
- Changed the representation of tile values from one byte encoding powers of 2 over to unsigned 32 bit integers.  This makes code simpler and easier to understand.
- Improved the build system to automatically select the best Curses implementation or allow the user to select their own when using Curses.
- Fixed an incorrect message displayed when prompting for the next move in the stdio backend.

##[0.3.1] - 2026-04-22
### Added
- A proper autotools based build system.
### Changed
- Added a temporary hack to ensure the C version compiles for Windows with PDCurses.  Will replace this with a better, more comprehensive fix later.

## [0.3.0] - 2026-04-12
### Added
- A readme, changelog, and specification document.
- Now using the git version control system.
### Changed
- Refactored code and internal state representation to reduce code size and complexity.

## [0.2.0] - 2025-11-16
### Added
- Animated Output in the Curses backend in the C version.
### Changed
- Reworked the internal state representation to better support animated output in the C version.

## [0.1.0] - 2025-08-19
### Added
- Initial C version.
- Basic stdio input/output backend.
- Basic Curses/Ncurses/PDcurses backend.
