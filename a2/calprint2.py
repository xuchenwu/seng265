#!/usr/bin/env python3

import sys
import argparse
import datetime

##############################################################################
#                              3 core functions                              #
##############################################################################

#1
def read_file(filename):
    '''
        This function is to read different event categories from a given file
        into a list of lists.
    '''

    try:
        myfile = open(filename, 'r')
        lines = [line.strip() for line in myfile]

        # The following 3 kinds of lines in the file are useless here
        lines.remove('BEGIN:VCALENDAR')
        lines.remove('END:VCALENDAR')
        lines = [line for line in lines if (line[0:8] != 'VERSION:')]

        categories = ('\n'.join(lines)).split('END:VEVENT')                     # seperate each category (as string) by 'END:VEVENT'
        categories.remove('')                                                   # remove any empty categories
        categories = [cat.split('\n') for cat in categories]                    # split each muti-line string category into a list of lines
        categories = [[line for line in cat if ((line != '')                    # remove any empty lines as well as 'BEGIN:VEVENT'
            and (line != 'BEGIN:VEVENT'))] for cat in categories]
        myfile.close()
        return categories
    except FileNotFoundError:
        print("Where is the file?!")
        exit()
    except PermissionError:
        print("May I have the permission? Thank you.")
        exit()

#2
def store_events(categories):
    '''
        This function is to store all events under each category into a
        dictionary according to the date of each event.
    '''

    events = {}
    for cat in categories:
        dtm_ical_start = cat[0][8:]
        dtm_ical_end = cat[1][6:]
        # if the category is repetitive, read the until date
        # otherwise set the until datetime the same as the end datetime
        dtm_ical_until = (cat[2][32:47] if cat[2][0] == 'R' else cat[1][6:])
        summary = cat[-1][8:]
        location = cat[-2][9:]

        # construct 3 datetime objects from datetime strings
        dtm_obj_start = dtm_ical_to_obj(dtm_ical_start)
        dtm_obj_end = dtm_ical_to_obj(dtm_ical_end)
        dtm_obj_until = dtm_ical_to_obj(dtm_ical_until)

        # store all repetitive events of a category
        # into the dictionary under their own dates
        while dtm_obj_end <= dtm_obj_until:
            date = dtm_obj_start.date()
            if date not in events:
                events[date] = []                                               # make sure that the value that each key corresponds to is list type
            events[date].append((dtm_obj_start.time(),
                dtm_obj_end.time(), summary, location))
            dtm_obj_start = one_week_later(dtm_obj_start)
            dtm_obj_end = one_week_later(dtm_obj_end)

    # sort the list under each key based on two keys: start time and summary
    # where start time is prioritized over summary
    for date in events:
        events[date].sort(key = lambda tup: [tup[0], tup[2]])
    return events

#3
def print_events(events, start, end):
    '''
        This function is to print all the events in the dictionary whose date
        are within the given range (from start to end)
    '''

    # construct 2 date objects based on the start date and the end date
    range_start = dt_cmd_to_obj(start)
    range_end = dt_cmd_to_obj(end)

    # sort the keys (dates) and filter those within the range
    keys = list(events.keys())
    keys.sort()
    keys = [key for key in keys if (key >= range_start and key <= range_end)]

    # start printing if the key list is not empty
    try:
        # seperate this case because different dates have to be seperated by an
        # empty line in the output,
        # but there is no empty line at the begin of the output
        key = keys[0]
        dt_str = '{0} {1:0>2}, {2} ({3})'.format(month(key), key.day,
            key.year, weekday(key))
        print(dt_str)
        print('-' * len(dt_str))
        for event in events[key]:
            print('{0} to {1}: {2} [{3}]'.format(time(event[0]),
                time(event[1]), event[2], event[3]))

        for key in keys[1:]:
            print()
            dt_str = '{0} {1:0>2}, {2} ({3})'.format(month(key),
                key.day, key.year, weekday(key))
            print(dt_str)
            print('-' * len(dt_str))
            for event in events[key]:
                print('{0} to {1}: {2} [{3}]'.format(time(event[0]),
                    time(event[1]), event[2], event[3]))
    except IndexError:
        exit()

###############################################################################
#                           end of 3 core functions                           #
###############################################################################


###############################################################################
#                            some helper functions                            #
###############################################################################

# increment a datatime object by 7 days
def one_week_later(dtm_obj):
    seven_days = datetime.timedelta(days = 7)
    return dtm_obj + seven_days

# construct a datetime object based on a string of the format 'yyyymmddThhmmss'
def dtm_ical_to_obj(dtm_ical):
    (y, mo, d, h, mi, s) = (dtm_ical[0:4], dtm_ical[4:6], dtm_ical[6:8],
        dtm_ical[9:11], dtm_ical[11:13], dtm_ical[13:])
    return datetime.datetime(int(y), int(mo), int(d), int(h), int(mi), int(s))

# construct a date object based on a string of the format 'dd/mm/yyyy'
def dt_cmd_to_obj(dt_cmd):
    i = [pos for (pos, ch) in enumerate(dt_cmd) if ch == '/']                   # get the indices of '/' in the string
    (y, mo, d) = (dt_cmd[(i[1] + 1):], dt_cmd[(i[0] + 1):i[1]], dt_cmd[:i[0]])
    return datetime.date(int(y), int(mo), int(d))

# get the string representation of the month of a date object
def month(dt_obj):
    months = ['January', 'February', 'March', 'April', 'May', 'June', 'July',
                'August', 'September', 'October', 'November', 'December']
    return months[dt_obj.month - 1]

# get the string representation of the weekday of a date object
def weekday(dt_obj):
    weekdays = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
    return weekdays[dt_obj.weekday()]

# get the string representation of a time object
def time(tm_obj):
    h = tm_obj.hour if (tm_obj.hour <= 12) else tm_obj.hour - 12
    h = 12 if h == 0 else h
    m = tm_obj.minute
    p = 'am' if (tm_obj.hour < 12) else 'pm'
    return '{0: >2}:{1:0>2} {2}'.format(h, m, p)

###############################################################################
#                            end of helper methods                            #
###############################################################################


# The code below configures the argparse module for use with
# assignment #2.
#
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', type=str, help='file to be processed')
    parser.add_argument('--start', type=str, help='start of date range')
    parser.add_argument('--end', type=str, help='end of data range')

    args = parser.parse_args()

    #print ('file: {}; start: {}; end: {}'.format( args.file, args.start,
    #    args.end))

    if not args.file:
        print("Need --file=<ics filename>")

    if not args.start:
        print("Need --start=dd/mm/yyyy")

    if not args.end:
        print("Need --end=dd/mm/yyyy")

    entries = read_file(args.file)
    events = store_events(entries)
    print_events(events, args.start, args.end)


if __name__ == "__main__":
    main()
