Import('env')
import datetime

# 固定使用北京时间 UTC+8（中国不实行夏令时）
now = datetime.datetime.utcnow() + datetime.timedelta(hours=8)
version = now.strftime("%Y%m%d")
date_str = now.strftime("%Y-%m-%d %H:%M:%S")

# 用 BUILD_FLAGS 直接传递 -D 参数，避免 SCons CPPDEFINES 引号转义问题
env.Append(BUILD_FLAGS=[
    '-DBUILD_VERSION=\\"{}\\"'.format(version),
    '-DBUILD_DATE=\\"{}\\"'.format(date_str)
])

print("BuildInfo: version={}, date={}".format(version, date_str))
