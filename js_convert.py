import os

PATH = os.path.join('..', 'apps')
files = os.listdir(PATH)

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
        app_name = js_file[:len(js_file)-3]
        header_file_name = 'js_app_' + app_name + '.h'
        with open(os.path.join(PATH, header_file_name), 'w') as header_file:
            header_file.write('#pragma once\n\n')
            header_file.write('const char js_app_' + app_name + '[] = \"' + content + '\";\n')
            print(js_file + ' converted to ' + header_file_name)
        
print('Finished')
