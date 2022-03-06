#!/usr/bin/env python

# -*- coding: utf-8 -*-

import re
from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
from conans.model.version import Version


class ModbusClientConan(ConanFile):
    name = "modbus"
    url = " https://github.com/cblauvelt/modbus"
    homepage = url
    author = "Christopher Blauvelt"
    description = "A C++ MODBUS library."
    license = "MIT"
    topics = ("modbus", "asio")
    exports = ["LICENSE"]
    exports_sources = ["CMakeLists.txt", "conan.cmake", "conanfile.py", "include/*", "src/*", "test/*"]
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    requires = "cpool/0.9.6", "boost/1.78.0", "fmt/8.1.1"
    build_requires = "gtest/cci.20210126"
    options = {"cxx_standard": [20], "build_testing": [True, False]}
    default_options = {"cxx_standard": 20, "build_testing": True}
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.settings.os == "Windows" and \
           self.settings.compiler == "Visual Studio" and \
           Version(self.settings.compiler.version.value) < "16":
            raise ConanInvalidConfiguration("modbus does not support MSVC < 16")

    def sanitize_version(self, version):
        return re.sub(r'^v', '', version)


    def set_version(self):
        git = tools.Git(folder=self.recipe_folder)
        self.version = self.sanitize_version(git.get_tag()) if git.get_tag(
        ) else "%s_%s" % (git.get_branch(), git.get_revision()[:12])

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_CXX_STANDARD"] = self.options.cxx_standard
        cmake.definitions["BUILD_TESTING"] = self.options.build_testing
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        self.copy("LICENSE", dst="licenses")
        self.copy("*.hpp", dst="include/modbus", src="include")
        self.copy("*.hpp", dst="include/modbus", src="include/client")
        self.copy("*.hpp", dst="include/modbus", src="include/core")
        self.copy("*.hpp", dst="include/modbus", src="include/server")
        self.copy("*.a", dst="lib", keep_path=False)
        
    def package_info(self):
        self.cpp_info.libs = ["modbus"]
