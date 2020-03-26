from conans import ConanFile, CMake, tools

class CorexConan(ConanFile):
    name = "execution_graph"

    python_requires = "conan-utils/1.0@corex/utils"
    python_requires_extend = ["conan-utils.BasePackage", "conan-utils.LibraryMixin"]

    def requirements(self):
        self.requires("cpp-taskflow/2.3.1@corex/thirdparty")
        self.requires("rttr/0.9.6.1@corex/thirdparty")

    def imports(self):
        self.copy("*", src="bin", dst=".bin")

    def build(self):
        self.cmake_build()
