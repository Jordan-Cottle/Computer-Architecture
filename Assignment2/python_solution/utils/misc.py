""" Module for miscellaneous utilities. """


def type_of(obj):
    """ Get type of an object as a string. """
    return obj.__class__.__qualname__
