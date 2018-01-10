'''
Author: Yu Lou
Date: 2017-02-23

Based on the answer by @enrico.bacis in http://stackoverflow.com/a/13207724/4398908
and @Phylliida in http://stackoverflow.com/a/31736883/4398908
'''

# Import modules
try:
    try:
        import termios, fcntl, sys, os, curses # Import modules for Linux
    except ImportError:
        import msvcrt # Import module for Windows
except ImportError:
    raise Exception('This platform is not supported.')

class KeyGetterLinux:
    '''
    Implemented kbhit(), getch() and getchar() in Linux.

    Tested on Ubuntu 16.10(Linux 4.8.0), Python 2.7.12 and Python 3.5.2
    '''
    def __init__(self):
        self.buffer = '' # A buffer to store the character read by kbhit
        self.started = False # Whether initialization is complete

    def kbhit(self, echo = False):
        '''
        Return whether a key is hitten.
        '''
        if not self.buffer:
            if echo:
                self.buffer = self.getchar(block = False)
            else:
                self.buffer = self.getch(block = False)

        return bool(self.buffer)

    def getch(self, block = True):
        '''
        Return a single character without echo.
        If block is False and no input is currently available, return an empty string without waiting.
        '''
        try:
            curses.initscr()
            curses.noecho()
            return self.getchar(block)
        finally:
            curses.endwin()

    def getchar(self, block = True):
        '''
        Return a single character and echo.
        If block is False and no input is currently available, return an empty string without waiting.
        '''
        self._start()
        try:
            return self._getchar(block)
        finally:
            self._stop()

    def _getchar(self, block = True):
        '''
        Return a single character and echo.
        If block is False and no input is currently available, return a empty string without waiting.
        Should be called between self._start() and self._end()
        '''
        assert self.started, ('_getchar() is called before _start()')

        # Change the terminal setting
        if block:
            fcntl.fcntl(self.fd, fcntl.F_SETFL, self.old_flags & ~os.O_NONBLOCK)
        else:
            fcntl.fcntl(self.fd, fcntl.F_SETFL, self.old_flags | os.O_NONBLOCK)

        if self.buffer: # Use the character in buffer first
            result = self.buffer
            self.buffer = ''
        else:
            try:
                result = sys.stdin.read(1)
            except IOError: # In python 2.7, using read() when no input is available will result in IOError.
                return ''

        return result

    def _start(self):
        '''
        Initialize the terminal.
        '''
        assert not self.started, '_start() is called twice'

        self.fd = sys.stdin.fileno()

        self.old_attr = termios.tcgetattr(self.fd)

        new_attr = termios.tcgetattr(self.fd)
        new_attr[3] = new_attr[3] & ~termios.ICANON
        termios.tcsetattr(self.fd, termios.TCSANOW, new_attr)

        self.old_flags = fcntl.fcntl(self.fd, fcntl.F_GETFL)

        self.started = True

    def _stop(self):
        '''
        Restore the terminal.
        '''
        assert self.started, '_start() is not called'

        termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.old_attr)
        fcntl.fcntl(self.fd, fcntl.F_SETFL, self.old_flags)

        self.started = False

    # Magic functions for context manager
    def __enter__(self):
        self._start()
        self.getchar = self._getchar # No need for self._start() now
        return self

    def __exit__(self, type, value, traceback):
        self._stop()
        return False

class KeyGetterWindows:
    '''
    kbhit() and getchar() in Windows.

    Tested on Windows 7 64 bit, Python 2.7.1
    '''
    def kbhit(self, echo):
        return msvcrt.kbhit()

    def getchar(self, block = True):
        if not block and not msvcrt.kbhit():
            return ''
        return msvcrt.getchar()

    def getch(self, block = True):
        if not block and not msvcrt.kbhit():
            return ''
        return msvcrt.getch()

    _getchar = getchar

    # Magic functions for context manager
    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        return False

try:
    import termios
    KeyGetter = KeyGetterLinux # Use KeyGetterLinux if termios exists
except ImportError:
    KeyGetter = KeyGetterWindows # Use KeyGetterWindows otherwise