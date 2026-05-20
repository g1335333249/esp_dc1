Import('env')
import datetime

# 固定使用北京时间 UTC+8（中国不实行夏令时）
now = datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=8)))
version = now.strftime("%Y%m%d")
date_str = now.strftime("%Y-%m-%d %H:%M:%S")

# 通过 build flags 直接定义宏，所有源文件无需 include 头文件即可使用
env.Append(CPPDEFINES=[
    ("BUILD_VERSION", '\\"{}\\"'.format(version)),
    ("BUILD_DATE", '\\"{}\\"'.format(date_str))
])

print("BuildInfo: version={}, date={}".format(version, date_str))
