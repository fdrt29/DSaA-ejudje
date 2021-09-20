import unittest
import module


class TestStringMethods(unittest.TestCase):
    def test_function_1(self):
        # Override the Python built-in input method
        module.input = lambda: '1\n2\n3\n'
        # Call the function you would like to test (which uses input)
        output = module()
        self.assertEqual(output, '6')
        assert output == '6'

    # def test_sum(self):
    #     lst = [1, 2, 3, 5, '\n']
    #     self.assertEqual('foo'.upper(), 'FOO')


unittest.main()
