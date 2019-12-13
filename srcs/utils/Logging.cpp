#include "Logging.hpp"

Logging		logging;

Logging::Logging()
: _loglevel(LOGDEBUG) {
	setLogColor(LOGDEBUG, COLOR_WHITE);
	setLogColor(LOGINFO, COLOR_WHITE);
	setLogColor(LOGSUCCESS, std::string(COLOR_GREEN) + COLOR_BOLD);
	setLogColor(LOGWARN, std::string(COLOR_YELLOW) + COLOR_BOLD);
	setLogColor(LOGERROR, std::string(COLOR_RED) + COLOR_BOLD);
	setLogColor(LOGFATAL, std::string(COLOR_RED) + COLOR_BOLD);

	setPrintFileLine(false);
}

Logging::Logging(Logging const &src) {
	*this = src;
}

Logging::~Logging() {
}

Logging & Logging::operator=(Logging const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void Logging::setLoglevel(eLoglevel loglevel) {
	_loglevel = loglevel;
}

void Logging::setLogColor(eLoglevel loglevel, std::string const & color) {
	_colors[loglevel] = color;
}

void Logging::setPrintFileLine(eLoglevel loglevel, bool printFileLine) {
	_printFileLine[loglevel] = printFileLine;
}
void Logging::setPrintFileLine(bool printFileLine) {
	for (int level = 0; level < static_cast<int>(NBLOG); level++) {
		setPrintFileLine(static_cast<eLoglevel>(level), printFileLine);
	}
}

void Logging::_savePrecision() {
	_precision = std::cout.precision();
}
void Logging::_setFileLine(std::string file, int line) {
	_file = file;
	_line = line;
}

void Logging::_log(int precision, std::ostream & out) const {
	eLoglevel level;
	if (precision < NBLOG) {
		level = static_cast<eLoglevel>(precision);
		// start logging
		out << getColor(level);
		if (level == LOGDEBUG) out << "[DEBUG]";
		else if (level == LOGINFO) out << "[INFO]";
		else if (level == LOGSUCCESS) out << "[SUCCESS]";
		else if (level == LOGWARN) out << "[WARN]";
		else if (level == LOGERROR) out << "[ERROR]";
		else if (level == LOGFATAL) out << "[FATAL]";
		if (_printFileLine[level])
			out << "[" << _file << " " << _line << "]";
		out << ": ";
	}
	else {
		level = static_cast<eLoglevel>(precision - NBLOG);
		// end logging
		out << COLOR_EOC << std::endl;
	}
}

std::ostream & operator << (std::ostream & out, const Logging & log) {
	// log
	logging._log(static_cast<int>(std::cout.precision()), out);
	// reset precision
	out << std::setprecision(log.getSavedPrecision());
	return out;
}

std::streamsize		Logging::getSavedPrecision() const { return _precision; }
std::string const &	Logging::getColor(eLoglevel loglevel) const { return _colors[loglevel]; }
eLoglevel			Logging::getLoglevel() const { return _loglevel; }
