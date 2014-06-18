TimeKeeper - A Qt Application for Time Tracking
=====

This is a little application we threw together for tracking the
time spent on different tasks during the work day.

Edit the list of tasks you might work on. Then, as you go through
your day, select a task and tell the application that you're starting
work, add notes as you go, then mark when you stop. At the end of
the day, ask for a report and your notes will be displayed along
with timestamps and a time summary.

The data are stored in date-stamped XML files in $HOME/timekeeper.
The files can be edited by hand *but only when timekeeper is not
running*.  The program does not handle transitions from one day to
the next properly, *so don't work through midnight*:-)
