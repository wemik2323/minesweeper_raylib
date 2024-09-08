import subprocess
import os
import shutil
import re
import stat

def remove_readonly(func, path, _):
    os.chmod(path, stat.S_IWRITE)
    func(path)

def RunConan(build_type):
    subprocess.run((
        'conan', 'install', '.', 
        '--build', 'missing', 
        '--output-folder=./.conan', 
        '--settings=compiler.cppstd=20',
        f'--settings=build_type={build_type}'
    ))

if __name__ == "__main__":
    RunConan("Debug")
    RunConan("Release")
