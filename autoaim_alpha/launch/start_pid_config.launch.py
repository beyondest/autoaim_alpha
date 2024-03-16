from launch import LaunchDescription
from launch_ros.actions import Node
package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package=package_name,
            node_executable='node_parameter',
            node_name='node_parameter',
            output='screen'
        )
    ])
