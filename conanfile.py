#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools
import os


class LibnameConan(ConanFile):
    name = "Simple-Web-Server"
    version = "v3.0.0-rc3"
    description = "A very simple, fast, multithreaded, platform independent HTTP and HTTPS server and client library implemented using C++11 and Asio (both Boost.Asio and standalone Asio can be used). Created to be an easy way to make REST resources available from C++ applications."
    # topics can get used for searches, GitHub topics, Bintray tags etc. Add here keywords about the library
    topics = ("c++11", "http", "rest")
    url = "https://github.com/inexorgame/conan-simple-web-server"
    homepage = "https://gitlab.com/eidheim/Simple-Web-Server"
    author = "Ole Christian Eidheim <eidheim@gmail.com>"
    license = "MIT"  # Indicates license type of the packaged library; please use SPDX Identifiers https://spdx.org/licenses/
    exports = ["LICENSE.md"]      # Packages the license for the conanfile.py
    # Remove following lines if the target lib does not use cmake.
    exports_sources = ["CMakeLists.txt"]
    generators = "cmake"

    # Options may need to change depending on the packaged library.
    settings = "os", "arch", "compiler", "build_type"
    options = {}
    default_options = {}
    #options = {"shared": [True, False], "fPIC": [True, False]}
    #default_options = {"shared": False, "fPIC": True}

    # Custom attributes for Bincrafters recipe conventions
    _source_subfolder = "source_subfolder"
    _build_subfolder = "build_subfolder"

    requires = (
        "asio/1.12.2@bincrafters/stable"
    )

    def config_options(self):
        pass

    def source(self):
        git = tools.Git(self._source_subfolder)
        git.clone("https://gitlab.com/eidheim/Simple-Web-Server.git")
        git.checkout("v3.0.0-rc3")
        
    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = "OFF"  # example
        cmake.configure(build_folder=self._build_subfolder)
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE", dst="licenses", src=self._source_subfolder)
        cmake = self._configure_cmake()
        cmake.install()
        # If the CMakeLists.txt has a proper install method, the steps below may be redundant
        # If so, you can just remove the lines below
        include_folder = os.path.join(self._source_subfolder, "include")
        self.copy(pattern="*", dst="include", src=include_folder)
        self.copy(pattern="*.dll", dst="bin", keep_path=False)
        self.copy(pattern="*.lib", dst="lib", keep_path=False)
        self.copy(pattern="*.a", dst="lib", keep_path=False)
        self.copy(pattern="*.so*", dst="lib", keep_path=False)
        self.copy(pattern="*.dylib", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
