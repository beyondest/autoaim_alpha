from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess




package_name = 'autoaim_alpha'

def generate_launch_description():
    return LaunchDescription(
        [    
         Node(
            package=package_name,
            node_executable='rqt_plot',
            node_name='plot_cur_tar_pitch',
            arguments=['--topics','/electric_sys_com/cur_pitch',
                       '--topics','/electric_sys_state/target_abs_pitch'],
            output='log'
        )]
    )
