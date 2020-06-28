import datetime

import pytz


def unix_dt(year, month, day, hour=0, minute=0, second=0):
    return pytz.UTC.localize(datetime.datetime(year, month, day, hour, minute, second))
