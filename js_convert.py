import os

PATH = os.path.join('..', 'apps')
header_dir = os.path.join(PATH, 'headers')

files = os.listdir(PATH)
if not os.path.exists(header_dir):
    os.makedirs(header_dir)
header_files = os.listdir(header_dir)
for header_file in header_files:
    os.remove(os.path.join(header_dir, header_file))

js_files = []
for f in files:
    if len(f) < 3:
        continue
    if f[-3:] == '.js':
        js_files.append(f)

for js_file in js_files:
    with open(os.path.join(PATH, js_file), 'r') as f:
        content = f.read()
        content = content.replace('\n', ' ')
        content = content.replace('\t', ' ')
        content = content.replace('"', '\"')
        content = content.replace("'", "\'")
        app_name = js_file[:len(js_file)-3]
        header_file_name = 'js_app_' + app_name + '.h'
        with open(os.path.join(PATH, 'headers', header_file_name), 'w') as header_file:
            header_file.write('#pragma once\n\n')
            header_file.write('const char js_app_' + app_name + '[] = \"' + content + '\";\n')
            print(js_file + ' converted to ' + header_file_name)

with open (os.path.join(header_dir, 'js_apps.h'), 'w') as f:
    f.write('#pragma once\n\n')
    for js_file in js_files:
        f.write('#include \"js_app_' + js_file[:len(js_file)-3] + '.h\"\n')
        
print('Finished')
