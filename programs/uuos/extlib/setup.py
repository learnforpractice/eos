import os
import sys
import platform
from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

data_files = [
    ('lib',['lib/libchain_api.dylib']),
    ('bin', ['bin/uuos']),
]

version = platform.python_version_tuple()
version = '%s.%s' % (version[0], version[1])

cur_dir = os.path.dirname(__file__)
test_dir = os.path.join(cur_dir, 'uuosio/tests')
for root, dirs, files in os.walk(test_dir):
    print(root)
    tests = []
    for file in files:
        if file.endswith('.pyc'):
            continue
        tests.append(os.path.join(root, file))
    install_dir = os.path.join(f'lib/python{version}/site-packages', root.replace(cur_dir, ''))
    data_files.append((install_dir, tests))

setup(
    name="uuosio",
    version="0.1.0",
    description="UUOSIO project",
    author='The UUOSIO Team',
    license="MIT",
    packages=['uuosio'],
    package_dir={'uuosio': 'uuosio/', 'uuosio/tests': 'uuosio/tests'},
    # package_data={'': ['data/hello.txt', 'data/hello2.txt', 'uuosio/tests/*']},
    data_files = data_files,
    scripts=['bin/run-uuos'],
    install_requires=[''],
    tests_require=['pytest'],
    setup_requires=setup_requires,
    include_package_data=True
)
