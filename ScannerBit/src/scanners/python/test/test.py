"""
Basic test of scanners, mocking gambit behaviour
================================================
"""

import unittest

from mock import mock_gambit


with mock_gambit():
    from plugins import plugins


class Test(unittest.TestCase):

    def test_all(self):
        
        skip = ["dynamic"]
    
        for k, v in plugins.items():

            if k in skip:
                continue
        
            inst = v()
            inst.run()


if __name__ == '__main__':
    unittest.main()