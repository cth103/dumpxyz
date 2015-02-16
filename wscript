APPNAME = 'dumpxyz'
VERSION = '0.0.4'

def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--target-windows', action='store_true', default=False, help='set up to do a cross-compile to make a Windows package')
    
def configure(conf):
    conf.load('compiler_cxx')

    conf.env.TARGET_WINDOWS = conf.options.target_windows

    if conf.env.TARGET_WINDOWS:
        boost_lib_suffix = '-mt'
    else:
        boost_lib_suffix = ''

    conf.check_cfg(package='libdcp-1.0', atleast_version='1.0.0', args='--cflags --libs', uselib_store='DCP', mandatory=True)
    conf.env.DEFINES_DCP = [f.replace('\\', '') for f in conf.env.DEFINES_DCP]
    conf.check_cxx(fragment="""
    			    #include <boost/filesystem.hpp>\n
    			    int main() { boost::filesystem::copy_file ("a", "b"); }\n
			    """, msg='Checking for boost filesystem library',
                            libpath='/usr/local/lib',
                            lib=['boost_filesystem%s' % boost_lib_suffix, 'boost_system%s' % boost_lib_suffix],
                            uselib_store='BOOST_FILESYSTEM')

def build(bld):
    bld.recurse('src')
