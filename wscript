
def options(opt):
    opt.load('compiler_cxx')
    
def configure(conf):
    conf.load('compiler_cxx')
    conf.check_cfg(package='libdcp-1.0', atleast_version='1.0.0', args='--cflags --libs', uselib_store='DCP', mandatory=True)
    conf.env.DEFINES_DCP = [f.replace('\\', '') for f in conf.env.DEFINES_DCP]
    conf.check_cxx(fragment="""
    			    #include <boost/filesystem.hpp>\n
    			    int main() { boost::filesystem::copy_file ("a", "b"); }\n
			    """, msg='Checking for boost filesystem library',
                            libpath='/usr/local/lib',
                            lib=['boost_filesystem', 'boost_system'],
                            uselib_store='BOOST_FILESYSTEM')

def build(bld):
    bld.recurse('src')
