#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#define COLOR_EOC	"\x1B[0m"
#define COLOR_RED  "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE  "\x1B[34m"
#define COLOR_MAGENTA  "\x1B[35m"
#define COLOR_CYAN  "\x1B[36m"
#define COLOR_WHITE  "\x1B[37m"
#define COLOR_F_RED  "\x1B[41m"
#define COLOR_F_GREEN  "\x1B[42m"
#define COLOR_F_YELLOW	"\x1B[43m"
#define COLOR_F_BLUE  "\x1B[44m"
#define COLOR_F_MAGENTA  "\x1B[45m"
#define COLOR_F_CYAN  "\x1B[46m"
#define COLOR_F_WHITE  "\x1B[47m"
#define COLOR_CLEAR "\033[H\033[2J"
#define COLOR_BOLD "\e[1m"
#define COLOR_LIGHT "\e[2m"
#define COLOR_ITALIC "\e[3m"
#define COLOR_ULINE "\e[4m"

enum	eLoglevel {
	LOGDEBUG, LOGINFO, LOGSUCCESS, LOGWARN, LOGERROR, LOGFATAL, NBLOG
};

#define _baselogStart(loglevel)	logging._savePrecision(); logging._setFileLine(__FILE__, __LINE__); \
	std::cout << std::setprecision(loglevel) << logging
#define _baselogEnd(loglevel)	std::setprecision(NBLOG + loglevel) << logging;

#define logDebug(x) { if (logging.getLoglevel() <= LOGDEBUG) \
    { _baselogStart(LOGDEBUG) << x << _baselogEnd(LOGDEBUG) } }
#define logInfo(x) { if (logging.getLoglevel() <= LOGINFO) \
    { _baselogStart(LOGINFO) << x << _baselogEnd(LOGINFO) } }
#define logSuccess(x) { if (logging.getLoglevel() <= LOGSUCCESS) \
    { _baselogStart(LOGSUCCESS) << x << _baselogEnd(LOGSUCCESS) } }
#define logWarn(x) { if (logging.getLoglevel() <= LOGWARN) \
    { _baselogStart(LOGWARN) << x << _baselogEnd(LOGWARN) } }
#define logErr(x) { if (logging.getLoglevel() <= LOGERROR) \
    { _baselogStart(LOGERROR) << x << _baselogEnd(LOGERROR) } }
#define logFatal(x) { if (logging.getLoglevel() <= LOGFATAL) \
    { _baselogStart(LOGFATAL) << x << _baselogEnd(LOGFATAL) } }
#define logErrExit(x) { if (logging.getLoglevel() <= LOGERROR) \
    { _baselogStart(LOGERROR) << x << _baselogEnd(LOGERROR); exit(1); } }
#define logFatalExit(x) { if (logging.getLoglevel() <= LOGFATAL) \
    { _baselogStart(LOGFATAL) << x << _baselogEnd(LOGFATAL); exit(1); } }

class Logging {
	public:
		Logging();
		Logging(Logging const &src);
		virtual ~Logging();

		Logging &			operator=(Logging const &rhs);

		void				setLoglevel(eLoglevel loglevel);  // don't cout under minLevel
		void				setLogColor(eLoglevel loglevel, std::string const & color);
		void				setPrintFileLine(eLoglevel loglevel, bool printFileLine);
		void				setPrintFileLine(bool printFileLine);

		void				_savePrecision();
		void				_setFileLine(std::string file, int line);
		void				_log(int level, std::ostream & out) const;

		std::streamsize		getSavedPrecision() const;
		std::string const &	getColor(eLoglevel loglevel) const;
		eLoglevel			getLoglevel() const;

	private:
		std::streamsize	_precision;
		std::string		_colors[NBLOG];
		bool			_printFileLine[NBLOG];
		eLoglevel		_loglevel;

		std::string		_file;
		int				_line;
};

std::ostream & operator << (std::ostream & out, Logging const & log);

extern eLoglevel	logging_loglevel;
extern Logging		logging;
