import os
import re
import json


def getVersionFromVcs():
    """
    request from vcs tag like:
    major.minor.patch[-pre-release][+metadata]-\\d+
    and parse it
    """
    myCmd = ''
    try:
        myCmd = os.popen('git describe --tags --long').read()
    except Exception as ex:
        print('Exception a:', ex)

    pattern = r'\d+(\.\d+){2}(-[\w.]+)?(\+[\w.]+)?-\d+'
    match = re.search(pattern, myCmd)
    return match[0] if match else ''


def parseVcsVersion(vs):
    """
    major - only digit require
    minor - only digit require
    patch - only digit require
    pre-release - -[a-zA-Z0-9_\\.] optional
    metadata - +[a-zA-Z0-9_\\.] optional
    revision - digit require
    """
    match = re.search(
        r'(\d+)\.(\d+)\.(\d+)(?:-([\w.]+))?(?:\+([\w.]+))?-(\d+)',
        vs
        )
    return match.groups() if match else ''



def buildVersionString(ver):
    """
      0     1      2      3             4       5
    major.minor.patch[-pre-release][+metadata].\\d+
    """
    semver = ''
    if not ver:
        return '0.0.0'

    sep = ('', '.', '.', '-', '+', '.')
    for x, item in enumerate(ver):
        if item is None:
            continue

        if x == 5 and item == '0':
            continue
        semver += sep[x] + item

    return semver


def getHashFromVcs():
    """get last commit hash and return 10 character"""
    myCmd = ''
    try:
        myCmd = os.popen('git show -s --format="%H %ad" --date=short').read()
    except Exception as ex:
        print('Exception b:', ex)
    hash_code = myCmd[:10]
    date = myCmd[-11:].replace('-', '').strip()
    return hash_code + ' ' + date


def buildVersionFile(cfg):
    ver_striing = getVersionFromVcs()
    match = parseVcsVersion(ver_striing)
    str_ver = buildVersionString(match)
    hash_date = getHashFromVcs()
    buildFromTemplate(cfg, str_ver, hash_date)


def buildFromTemplate(cfg, str_ver, hash_date):
    template = open(cfg['template_path'], 'r')

    ver_file = open(cfg['version_path'], 'w')

    for line in template:
        line = re.sub(
                '{{' + cfg['template_word'] + '}}',
                str_ver,
                line
            )
        line = re.sub(
                '{{' + cfg['template_hash'] + '}}',
                hash_date,
                line
            )
        ver_file.write(line)

    template.close()
    ver_file.close()


def readConfig():
    config = {
        "template_path": "./version_template.c",
        "template_word": "SEMVER",
        "template_hash": "HASH_DATA",
        "version_path": "./version.c"
    }
    try:
        with open("config.json", "r") as config_file:
            config = json.load(config_file)
    except OSError as ex:
        print('ERROR read configuration.', ex)
    except Exception as ex:
        print(ex)

    return config


if __name__ == '__main__':
    buildVersionFile(readConfig())
