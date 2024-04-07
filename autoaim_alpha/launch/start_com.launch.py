from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package="autoaim_alpha",
            node_executable='node_com',
            node_name='node_com',
            output='log'
        )
    ])
