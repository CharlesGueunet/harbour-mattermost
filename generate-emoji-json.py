#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import urllib.request
import json

final_json = []

# Mattermost WebApp Emoji URL from repo
mattermost_emoji_url = 'https://raw.githubusercontent.com/mattermost/mattermost-webapp/master/utils/emoji.json'

# Categories match
categories = { 'activity': 'Activity',
               'flags': 'Flags',
               'foods': 'Foods',
               'nature': 'Nature',
               'objects': 'Objects',
               'people': 'People',
               'places': 'Places',
               'skintone': 'Skin Tones',
               'symbols': 'Symbols',
               'custom': 'Other'
               }


# # read local file
# with open('mattermost-emoji.json') as emoji_json:
#     mattermost_emoji = json.load(emoji_json)

# get emoji.json from repo
response = urllib.request.urlopen(mattermost_emoji_url)
mattermost_emoji = json.loads(response.read())


for emoji in mattermost_emoji:
    image = emoji['filename']

    # remove -fe0f from end of short emoji
    if len(image) <= 10:
        image = emoji['filename'].replace('-fe0f', '')
    # remove -fe0f- from long emoji 
    else:
        image = emoji['filename'].replace('-fe0f-', '-')
    # remove leading zeroes
    if image.startswith( '00' ):
        image = image[2:]
    # custom emojis png only    
    if emoji['category'] == 'custom':
        filepath = 'png/' + image + '.png'
    else:
        filepath = 'svg/' + image + '.svg'

    emoji_item = { 'category': categories[ emoji['category'] ], 'image': filepath , 'short_names': emoji['aliases'] }
    final_json.append(emoji_item)
    # # print every generated JSON item
    # print( json.dumps(emoji_item, indent=4) )

# # print generated JSON
# print( json.dumps(final_json, indent=4) )

# save generated JSON to file
with open('resources/emoji/emoji.json', 'w') as outfile:
    json.dump(final_json, outfile, indent=4)
