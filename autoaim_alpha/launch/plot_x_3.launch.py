from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

## plot x,y only work if you set if_pub_armor_pos* = True in __init__.py
## plot x,y only work for one armor situation
# y axis towards back
# x axis towards right hand 




# (my)pose [ [pose[position[x,y,z],orientation[x,y,z,w]]], [header]]


package_name = 'autoaim_alpha'

def generate_launch_description():
    return LaunchDescription(
        [    
         Node(
            package=package_name,
            node_executable='rqt_plot',
            node_name='plot_cur_tar_yaw',
            arguments=['--topics','/electric_sys_com/reach_unix_time',
                       '--topics','/electric_sys_state/unix_time'],
            output='log'
        )]
    )
    
    
    
