#
# UVic SENG 265, Summer 2019, Assignment #3
#
# Student work is to appear in this module.
#

import re
import datetime

class Calprint:
    '''
    This class reads and stores the content of a iCal file.
    Different events are stored by dates.

    Attributes
    ----------
    self.__events_by_day : dictionaries
        mapps date to events happening on that date
    self.__filename : str
        the name of the iCal file from which the content is read and stored

    Methods
    -------
    get_events_for_day(dt)
    __read_file()
    __store_events(content)  
    '''

    #================================================ Built-in Methods ================================================#
    
    def __init__(self, filename):
        '''
        Constructor.
        
        Parameter
        ---------
        filename : str
            the name of the file to be read
        '''

        self.__events_by_day = {}
        self.__filename = filename
        self.__store_events(self.__read_file())

    #================================================ Public Methods ================================================#
    
    def get_events_for_day(self, dt):
        '''
        Returns the events on the given date.

        Parameter
        ---------
        dt : datetime.datetime
            the date of the events asked for

        Returns
        -------
        Events
            an Events object which represents the events happening on the given date
        '''

        try:
            return self.__events_by_day[dt]
        except KeyError:
            return None

    #================================================ Private Methods ================================================#
    
    def __read_file(self):
        '''
        Reads the content in self.__filename.

        Returns
        -------
        list
            A list of lists, where each list element stores the properties of some type of event
        '''

        try:
            myfile = open(self.__filename, 'r')
            lines = [line.strip() for line in myfile]

            # The following 3 kinds of lines in the file are useless here
            lines.remove('BEGIN:VCALENDAR')
            lines.remove('END:VCALENDAR')

            pattobj1 = re.compile('VERSION:')
            pattobj2 = re.compile('BEGIN:VEVENT')
            lines = [line for line in lines if not pattobj1.match(line)]

            content = ('\n'.join(lines)).split('END:VEVENT')        # seperate each category (as string) by 'END:VEVENT'
            content.remove('')                                      # remove any empty categories
            content = [entry.split('\n') for entry in content]      # split each muti-line string category into a list of lines
            content = [[line for line in entry if not ((line == '') # remove any empty lines as well as 'BEGIN:VEVENT'
                or (pattobj2.match(line)))] for entry in content]
            myfile.close()
            return content
        except FileNotFoundError:
            print("Where is the file?!")
            exit()
        except PermissionError:
            print("May I have the permission? Thank you.")
            exit()

    def __store_events(self, content):
        '''
        Stores the events into self.__events_by_day.

        Parameter
        ---------
        content : list
            A list of lists, where each list is a list of strings which has the form
            ['DTSTART:...', 'DTSTART:...', 'LOCATION:...', 'SUMMARY:...'] or
            ['DTSTART:...', 'DTSTART:...', 'RRULE:...', 'LOCATION:...', 'SUMMARY:...']
        '''

        pattobj3 = re.compile(r'(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})') # pattern for date and time string
        pattobj4 = re.compile(r'LOCATION:(.*)')                        # pattern for location string
        pattobj5 = re.compile(r'SUMMARY:(.*)')                         # pattern for summary string
        for entry in content:
            # extract start date and time
            mat = pattobj3.search(entry[0])
            (y_start, mo_start, d_start, h_start, mi_start) = mat.groups()
            # extract end time
            mat = pattobj3.search(entry[1])
            h_end, mi_end = mat.group(4), mat.group(5)
            # extract until date if exists, otherwise equate it with start date
            mat = pattobj3.search(entry[2])
            (y_until, mo_until, d_until) = (mat.group(1), mat.group(2),
                mat.group(3)) if mat else (y_start, mo_start, d_start)
            # extract location
            mat = pattobj4.search(entry[-2])
            location = mat.group(1)
            # extract summary
            mat = pattobj5.search(entry[-1])
            summary = mat.group(1)
            # construct an Event object representing an event with particular properties
            event = Event(int(y_start), int(mo_start), int(d_start), int(h_start),
                int(mi_start), int(h_end), int(mi_end), location, summary)
            # construct a datetime object representing the until date
            dt_until_obj = datetime.datetime(int(y_until), int(mo_until), int(d_until))
            # store all events of a certain type from the start date to the until date
            # into a dictionary where the date is the key
            while event.get_date() <= dt_until_obj:
                try:
                    self.__events_by_day[event.get_date()].add_event(event)
                except KeyError:
                    self.__events_by_day[event.get_date()] = Events(event)
                event =  event.repeat_by_week()

class Event:
    '''
    This class represents a single event.

    Attributes
    ----------
    self.__dt : datetime.datetime
        the date on which the event happens
    self.__t_start : datetime.time
        the time when the event starts
    self.__t_end : datetime.time
        the time when the event ends
    self.__location : str
        the location where the event takes place
    self.__summary : str
        the description of the event

    Methods
    -------
    get_date()
    repeat_by_week()
    __time(t)
    '''

    #================================================ Built-in Methods ================================================#
    
    def __init__(self, y, mo, d, h_start, mi_start, h_end, mi_end, location, summary):
        '''
        Constructor.

        Parameters
        ----------
        y : int
            year
        mo : int
            month
        d : int
            day
        h_start : int
            start hour
        mi_start : int
            start minute
        h_end : int
            end hour
        mi_end : int
            end minute
        location : str
            the location where the event takes place
        summary : str
            the description of the event
        '''

        self.__dt = datetime.datetime(y, mo, d)
        self.__t_start = datetime.time(h_start, mi_start)
        self.__t_end = datetime.time(h_end, mi_end)
        self.__location = location
        self.__summary = summary

    def __repr__(self):
        '''
        Returns the string representation of the event.

        Returns
        -------
        str
            a string of the form 
            '<start hour>:<start minute> <am/pm> to <end hour>:<end minute> <am/pm>: <summary> [<location>]'
        '''
        return self.__time(self.__t_start) + ' to ' + self.__time(self.__t_end) + ': ' + self.__summary + ' [' + self.__location + ']'

    def __eq__(self, other):
        '''
        This method defines equality of two Event objects.

        Returns
        -------
        boolean
            true if two events have identical start time and summary 
        '''

        return self.__t_start == other.__t_start and self.__summary == other.__summary

    def __lt__(self, other):
        '''
        This method defines inequality of two Event object.s

        Returns
        -------
        boolean
            true if the first event has early start time, 
            or two events have the same start time and the first events has smaller summary string according to dictionary order 
        '''

        if self.__t_start == other.__t_start:
            return self.__summary < other.__summary
        return self.__t_start < other.__t_start

    #================================================ Public Methods ================================================#
    
    def get_date(self):
        '''
        Returns
        -------
        datetime.datetime
            self.__dt
        '''

        return self.__dt

    def repeat_by_week(self):
        '''
        Creates an event of the same type which happens one week later.

        Returns
        -------
        Event
            an event of the same type as self but which happens one week later
        '''

        dt = self.__dt + datetime.timedelta(days = 7)
        y, mo, d = dt.year, dt.month, dt.day
        h_start, mi_start = self.__t_start.hour, self.__t_start.minute
        h_end, mi_end = self.__t_end.hour, self.__t_end.minute
        location = self.__location
        summary = self.__summary
        return Event(y, mo, d, h_start, mi_start, h_end, mi_end, location, summary)

    #================================================ Private Methods ================================================#
    
    def __time(self, t):
        '''
        Produce a string representation of a time.

        Parameter
        ---------
        t : datetime.time
            the time object to be represented
        
        Returns
        -------
        str
            a string of the form '<hour>:<minute> <am/pm>'
        '''

        h = t.hour if (t.hour <= 12) else (t.hour - 12)
        h = 12 if h == 0 else h
        m = t.minute
        p = 'am' if (t.hour < 12) else 'pm'
        return '{0: >2}:{1:0>2} {2}'.format(h, m, p)


class Events:
    '''
    This class represents a list of events which happens on the same date.

    Attributes
    ----------
    self.__events : list
        a list of events which happen on the same date
    self.__date : datetime.datetime:
        the date on which the events happen

    Methods
    -------
    add_event(event)
    __rep_date()
    __rep_month()
    __rep_weekday()
    '''

    #================================================ Built-in Methods ================================================#

    def __init__(self, event):
        '''
        Constructor.

        Parameter
        ---------
        event : Event
            an event to put into the list
        '''

        self.__events = [event]
        self.__date = event.get_date()

    def __repr__(self):
        '''
        Produces a string representation of the events in order.

        Returns
        -------
        str
            form:
                <month> <day>, <year> (<weekday>)
                ---------------------------------
                event1
                event2
                ...
        '''

        output = self.__rep_date() + '\n' + '-' * len(self.__rep_date())
        for event in sorted(self.__events):
            output = output + '\n' + str(event)
        return output

    #================================================ Public Methods ================================================#
   
    def add_event(self, event):
        '''
        Adds an event to self.__events.

        Parameter
        ---------
        event : Event
            the event to be added
        '''

        self.__events.append(event)
    
    #================================================ Private Methods ================================================#
    
    def __rep_date(self):
        '''
        Produces a string representation of self.__date.

        Returns
        -------
        str
            form: '<month> <day>, <year> (<weekday>)
        '''

        return '{0} {1:0>2}, {2} ({3})'.format(self.__rep_month(), self.__date.day, self.__date.year, self.__rep_weekday())

    def __rep_month(self):
        '''
        Gives the string of month.

        Returns
        -------
        str
        '''

        months = ['January', 'February', 'March', 'April', 'May', 'June', 'July',
                    'August', 'September', 'October', 'November', 'December']
        return months[self.__date.month - 1]

    def __rep_weekday(self):
        '''
        Gives the first three chars of the string of weekday.

        Returns
        -------
        str
        '''

        weekdays = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
        return weekdays[self.__date.weekday()]

