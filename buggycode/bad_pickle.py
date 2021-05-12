# contains bunch of buggy examples
import subprocess
import base64
import cPickle


"""
input injection
"""
def transcode_file(filename):
    command = 'ffmpeg -i "{source}" output_file.mpg'.format(source=filename)
    subprocess.call(command, shell=True)  # a bad idea!


"""
assert statements
"""
def current_function(user):
    assert user.is_admin, 'user does not have access'
    # secure code...


"""
doing something
"""
class RunBinSh:
    def reduce_function(self):
        return (subprocess.Popen, (('/bin/sh',),))


print(base64.b64encode(cPickle.dumps(RunBinSh())))
