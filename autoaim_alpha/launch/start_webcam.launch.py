from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package="autoaim_beta",
            node_executable='node_webcam',
            node_name='node_webcam',
            output='log'
        )
    ])
