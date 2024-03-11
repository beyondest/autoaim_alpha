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
            arguments=['--topics','/armor_pos_without_correct/pose/pose/position/z',
                       '--topics','/armor_pos_corrected/pose/pose/position/z',
                       '--topics','/armor_pos_predicted/pose/pose/position/z'],
            output='log'
        )]
    )
    


