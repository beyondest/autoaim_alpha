from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess


package_name = 'rqt_plot'


def generate_launch_description():
    return LaunchDescription(
        [    
         Node(
            package=package_name,
            node_executable='rqt_plot',
            node_name='plot_cur_tar_yaw',
            arguments=['--topics','/electric_sys_com/cur_yaw',
                       '--topics','/electric_sys_state/target_abs_yaw'],
            output='log'
        )]
    )
