import datetime


def unix_dt(year, month, day, hour=0, minute=0, second=0, microsecond=0):
    return datetime.datetime(year, month, day, hour, minute, second, microsecond, tzinfo=datetime.timezone.utc)
