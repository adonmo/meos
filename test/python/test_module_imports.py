def test_module_imports():
    """
    This test checks that __module__ is set properly, i.e, as 'pymeos' and not '_pymeos'.
    This is necessary for Sphinx to generate documentation properly.
    """

    import pymeos

    assert pymeos.__name__ == 'pymeos'
    assert pymeos.GeomPoint.__name__ == 'GeomPoint'
    assert pymeos.GeomPoint.__module__ == 'pymeos'

    import pymeos.box

    assert pymeos.box.__name__ == 'pymeos.box'
    assert pymeos.box.__module__ == 'pymeos.box'

    from pymeos.box import TBox

    assert TBox.__name__ == 'TBox'
    assert TBox.__module__ == 'pymeos.box'
