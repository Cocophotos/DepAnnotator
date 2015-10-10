#! /usr/bin/env python
# -*- encoding: utf-8 -*-
import os, re, sys, codecs

print "You need to run this script as root"
print "If it's not the case, please rerun the script as root"
print "The script will ask you a few question about the program"

def ask_for_path():
    running = True
    while running:
        print "Where is your application located?"
        app_path = raw_input("> ")
        if os.path.exists(app_path):
            return app_path
        else:
            print "Sorry but the path does not exists"

def ask_for_appname(app_path):
    running = True
    while running:
        print "What is the exact name of your application?"
        appname = raw_input("> ")
        if os.path.exists( os.path.join(app_path, appname) ):
            return os.path.join(app_path, appname)
        else:
            print "Sorry but %s does not seem to exist" % ( os.path.join(app_path, appname) )

def ask_for_icon_path():
    running = True
    while running:
        print "What is the icon's path?"
        icon_path = raw_input("> ")
        if os.path.exists( icon_path ):
            return icon_path
        else:
            print "Sorry but %s does not seem to exist" % ( icon_path )

def generate_desktop_file( icon_path, app_complete_path ):
    name = os.path.splitext(os.path.basename(app_complete_path))[0]
    desktop = """[Desktop Entry]
    Version=1.0
    Type=Application
    Terminal=false
    StartupNotify=true
    Icon=%s
    Name=%s
    Comment=
    Exec=env UBUNTU_MENUPROXY=0 %s
    Categories=Application;""" % (icon_path, name, app_complete_path )

    desktop = desktop.encode('utf-8')

    with codecs.open("/usr/share/applications/%s.desktop" % name, 'w', 'utf-8') as stream:
        stream.write(desktop)
        print "Everything's fine now."


def main():
    running = True
    try:
        while running:
            print "Shall we start? [Y/n]"
            res = raw_input("> ")
            if res.lower() in ['y', 'n']:
                if res.lower() == 'n':
                    running = False
                else:
                   generate_desktop_file( ask_for_icon_path(), ask_for_appname( ask_for_path() ) )
                   running = False
            else:
                print "Please, answer by Y or n"
    except EOFError:
        pass
    finally:
        print "Exit the program"

if __name__ == '__main__':
    main()
