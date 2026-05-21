Import('env')
import datetime

# 固定使用北京时间 UTC+8（中国不实行夏令时）
now = datetime.datetime.utcnow() + datetime.timedelta(hours=8)
version = now.strftime("%Y%m%d")
date_str = now.strftime("%Y-%m-%dT%H:%M:%S")

# 通过 CPPDEFINES 定义宏，value 用 '"xxx"' 形式确保 C 预处理器将其识别为字符串字面量
env.Append(CPPDEFINES=[
    ("BUILD_VERSION", '"{}"'.format(version)),
    ("BUILD_DATE", '"{}"'.format(date_str))
])

print("BuildInfo: version={}, date={}".format(version, date_str))
